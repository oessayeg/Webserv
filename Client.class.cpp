#include "MainHeader.hpp"

Client::Client( void ) : _socket(0), bytesRead(0),\
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false), shouldReadBody(false), errString(), \
		finishedBody(false), gotFileName(false), shouldSkip(false), \
		bytesToRead(0), bytesCounter(0), contentLength(0), bodyType(0) { }

Client::Client( const Client &rhs )
{
	*this = rhs;
}

Client &Client::operator=( const Client &rhs )
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->bytesRead = rhs.bytesRead;
		*this->request = *rhs.request;
		this->isRead = rhs.isRead;
		this->isRqLineParsed = rhs.isRqLineParsed;
		this->isHeaderParsed = rhs.isHeaderParsed;
		this->shouldReadBody = rhs.shouldReadBody;
		this->finishedBody = rhs.finishedBody;
		this->gotFileName = rhs.gotFileName;
		this->shouldSkip = rhs.shouldSkip;
		this->bytesToRead = rhs.bytesToRead;
		this->correspondingBlock = rhs.correspondingBlock;
		this->bytesCounter = rhs.bytesCounter;
		this->clientStruct = new struct sockaddr_in;
		*this->clientStruct = *rhs.clientStruct;
		this->stringRequest = rhs.stringRequest;
		this->boundary = rhs.boundary;
		this->errString = rhs.errString;
		this->contentLength = rhs.contentLength;
		this->parsedRequest = rhs.parsedRequest;
		this->bodyType = rhs.bodyType;
	}
	return *this;
}

Client::~Client( void )
{
	delete this->clientStruct;
}

int Client::getSocket( void ) 
{
	return (_socket);
}

void Client::setSocket( int s )
{
	_socket = s;
}

void Client::checkRequestLine( void )
{
	std::string errorResponse;

	this->clientResponse.setBool(true);
	if (!parsedRequest.hasAllowedChars())
		this->clientResponse.setResponse(formError(400, "HTTP/1.1 400 Bad Request\r\n", "Error 400 Bad Request"));
	else if (!parsedRequest.hasGoodSize())
		this->clientResponse.setResponse(formError(414, "HTTP/1.1 414 Request-URI too long\r\n", "Error 414 Uri Too Long"));
	else if (!parsedRequest.isSupported())
		this->clientResponse.setResponse(formError(405, "HTTP/1.1 405 Method Not Allowed\r\n", "Error 405 Method Not Allowed"));
	else if (!parsedRequest.isGoodVersion())
		this->clientResponse.setResponse(formError(505, "HTTP/1.1 505 Version Not Supported\r\n", "Error 505 Version Not Supported"));
	else
		this->clientResponse.setBool(false);
}

void Client::checkHeaders( void )
{
	std::istringstream s(parsedRequest._headers["Content-Length"]);
	std::string transferEnc;
	std::string contentType;

	// This first part checks if there is an error
	s >> contentLength;
	transferEnc = parsedRequest._headers["Transfer-Encoding"];
	contentType = parsedRequest._headers["Content-Type"];
	clientResponse.setBool(true);
	if (contentLength > correspondingBlock->maxBodySize)
		clientResponse.setResponse(formError(413, "HTTP/1.1 413 Content Too Large\r\n", "Error 413 Content Too Large"));
	else if ((!transferEnc.empty() && transferEnc != "chunked")
		|| (transferEnc == "chunked" && contentType.find("multipart") != std::string::npos))
		clientResponse.setResponse(formError(501, "HTTP/1.1 501 Not Implemented\r\n", "Error 501 Not Implemented"));
	else if (parsedRequest._method == "POST" && contentLength == 0 && transferEnc.empty())
		clientResponse.setResponse(formError(400, "HTTP/1.1 400 Bad Request\r\n", "Error 400 Bad Request"));
	else
		clientResponse.setBool(false);
	if (clientResponse.getBool() || parsedRequest._method != "POST")
		return ;
	// This second part will set some useful variables for the type of body reading
	if (transferEnc == "chunked")
	{
		this->shouldReadBody = true;
		this->bodyType = CHUNKED;
	}
	else if (contentType.find("multipart/form-data;") != std::string::npos)
	{
		int index = contentType.find("boundary=") + 9;
		this->boundary = "--" + contentType.substr(index, contentType.size() - index);
		this->shouldReadBody = true;
		this->bodyType = MULTIPART;
	}
	else if (contentLength > 0)
	{
		this->shouldReadBody = true;
		this->bodyType = OTHER;
	}
}

// This function forms the whole response when an error happens
std::string Client::formError( int statusCode, const std::string &statusLine, const std::string &msgInBody )
{
	std::map< int, std::string >::iterator b;
	std::string returnString, fileInString;
	std::ifstream errorFile;
	std::stringstream s;

	b = correspondingBlock->errorMap.find(statusCode);
	if (b != correspondingBlock->errorMap.end())
		errorFile.open(b->second);
	if (b != correspondingBlock->errorMap.end() && !errorFile.is_open())
	{
		returnString = "HTTP/1.1 500 Internal Server Error\r\n";
		errString.setErrorFile("Error 500 Internal Server Error");
		s << errString.getFileInString().size();
		returnString += "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n\r\n";
		return (returnString + errString.getFileInString());
	}
	else if (b != correspondingBlock->errorMap.end() && errorFile.is_open())
	{
		fileInString = std::string((std::istreambuf_iterator<char>(errorFile)), (std::istreambuf_iterator<char>()));
		s << fileInString.size();
		returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n\r\n";
		return returnString + fileInString;
	}
	errString.setErrorFile(msgInBody);
	s << errString.getFileInString().size();
	returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str();
	returnString += "\r\n\r\n";
	return (returnString + errString.getFileInString());
}

// void Client::checkBody( const std::string &key, const std::string &value )
// {
// 	int index;

// 	if (parsedRequest._method != "POST")
// 		return ;
// 	if ((key == "Transfer-Encoding" && value == "chunked"))
// 		this->shouldReadBody = true;
// 	else if (key == "Content-Length")
// 	{
// 		std::istringstream s(value);

// 		s >> contentLength;
// 		if (contentLength > 0)
// 			this->shouldReadBody = true;
// 	}
// 	else if (key == "Content-Type" && value.find("multipart/form-data;") != std::string::npos)
// 	{
// 		index = value.find("boundary=") + 9;
// 		this->boundary = "--" + value.substr(index, value.size() - index);
// 		this->shouldReadBody = true;
// 	}
// }

// void Client::openFile( char *name )
// {
// 	char *fileName;
// 	int i;

// 	for (i = 0; name[i] != '\"'; i++);
// 	fileName = new char[i + 1];
// 	for (i = 0; name[i] != '\"'; i++)
// 		fileName[i] = name[i];
// 	fileName[i] = '\0';
// 	fileToUpload.open(fileName, std::ios::trunc | std::ios::binary);
// 	delete fileName;
// 	return ;
// }

// void Client::openWithProperExtension( void )
// {
// 	std::string extension;

// 	extension = extensions.getExtension(parsedRequest._headers["Content-Type"]);
// 	fileToUpload.open(randomString() + extension, std::ios::trunc | std::ios::binary);
// }

// std::string Client::randomString( void )
// {
//     std::string tmp_s;
//     const char alphanum[] =
//         "0123456789"
//         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//         "abcdefghijklmnopqrstuvwxyz";

// 	srand(time(NULL));
//     tmp_s.reserve(8);
//     for (int i = 0; i < 8; ++i)
//         tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
//     return tmp_s;
// }

// void Client::parseChunkedMultipart( void )
// {
// 	int endOfBody;

// 	if (!this->isThereFilename(CHUNKED_MULTIPART))
// 		return ;
// 	if (bytesToRead == 0 && stringRequest.find("\r\n") == std::string::npos)
// 		return ;
// 	else if (bytesToRead == 0 && isEndOfBody())
// 	{
// 		gotFileName = false;
// 		shouldSkip = false;
// 		fileToUpload.close();
// 	}
// 	else if (bytesToRead == 0 && gotFileName)
// 	{
// 		if (stringRequest[0] == '\r')
// 			stringRequest.erase(0, 2);
// 		bytesToRead = giveDecimal(stringRequest);
// 		stringRequest.erase(0, stringRequest.find('\n') + 1);
// 	}
// 	if (bytesToRead > 0 && stringRequest.size() > 0 && stringRequest.size() < bytesToRead)
// 	{
// 		if (!shouldSkip)
// 			fileToUpload << stringRequest;
// 		bytesToRead -= stringRequest.size();
// 		stringRequest.erase();
// 		return ;
// 	}
// 	else if (bytesToRead > 0 && stringRequest.size() > 0 && stringRequest.size() >= bytesToRead)
// 	{
// 		if (!shouldSkip)
// 			fileToUpload << stringRequest.substr(0, bytesToRead);
// 		stringRequest.erase(0, bytesToRead);
// 		bytesToRead = 0;
// 		if (stringRequest.size() < 9)
// 			return ;
// 	}
// 	parseChunkedMultipart();
// }

// char *Client::giveBody( char *limiter )
// {
// 	int i;
// 	char *retString;

// 	for (i = 0; request + i != limiter; i++);
// 	retString = new char[i + 1];
// 	for (i = 0; request + i != limiter; i++)
// 		retString[i] = request[i];
// 	retString[i] = '\0';
// 	return retString;
// }

// char *Client::giveDelimiter( void )
// {
// 	char *retString;
// 	int i;

// 	for (i = 0; request[i] != '\0' && request[i] != '\r'; i++);
// 	retString = new char[i + 1];
// 	for (i = 0; request[i] != '\0' && request[i] != '\r'; i++)
// 		retString[i] = request[i];
// 	retString[i] = '\0';
// 	return retString;
// }

// bool Client::isEndOfBody( void )
// {
// 	std::string hexStr;
// 	size_t decimal;

// 	if (stringRequest.size() <= 9)
// 		return false;
// 	if (stringRequest[0] == '\r' && stringRequest[1] == '\n')
// 		hexStr = stringRequest.substr(2, stringRequest.find('\r', 2) - 2);
// 	decimal = giveDecimal(hexStr);
// 	if (decimal == 2 && stringRequest.substr(5, 4) == "\r\n\r\n")
// 	{
// 		stringRequest.erase(0, 9);
// 		return true;
// 	}
// 	else if (decimal == (boundary.size() + 6))
// 	{
// 		decimal = stringRequest.find('\r', 2);
// 		if (stringRequest.substr(decimal + 4, stringRequest.find('\r', decimal + 4) - (decimal + 4))
// 			== (boundary + "--"))
// 		{
// 			std::cout << "Finished Body" << std::endl;
// 			finishedBody = true;
// 		}
// 		return true;
// 	}
// 	return false;
// }