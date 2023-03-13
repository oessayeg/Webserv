#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0),\
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false), shouldReadBody(false), errString(), \
		finishedBody(false), gotFileName(false), shouldSkip(false), \
		bytesToRead(0), bytesCounter(0) { }

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
		this->body = rhs.body;
		this->errString = rhs.errString;
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
	size_t contentLen;
	std::istringstream s(parsedRequest._headers["Content-Length"]);

	s >> contentLen;
	if (parsedRequest._headers.find("Content-Length") != parsedRequest._headers.end() && contentLen > correspondingBlock->maxBodySize)
	{
		clientResponse.setResponse(formError(413, "HTTP/1.1 413 Content Too Large\r\n", "Error 413 Content Too Large"));
		clientResponse.setBool(true);
	}
	else if (parsedRequest._headers.find("Transfer-Encoding") != parsedRequest._headers.end()
		&& parsedRequest._headers["Transfer-Encoding"] != "chunked")
	{
		clientResponse.setResponse(formError(501, "HTTP/1.1 501 Not Implemented\r\n", "Error 501 Not Implemented"));
		clientResponse.setBool(true);
	}
	else if (parsedRequest._method == "POST" && ((parsedRequest._headers.find("Content-Length")
		== parsedRequest._headers.end() && parsedRequest._headers.find("Transfer-Encoding")
		== parsedRequest._headers.end()) || contentLen == 0))
	{
		clientResponse.setResponse(formError(400, "HTTP/1.1 400 Bad Request\r\n", "Error 400 Bad Request"));
		clientResponse.setBool(true);
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

void Client::openFile( char *name )
{
	char *fileName;
	int i;

	for (i = 0; name[i] != '\"'; i++);
	fileName = new char[i + 1];
	for (i = 0; name[i] != '\"'; i++)
		fileName[i] = name[i];
	fileName[i] = '\0';
	fileToUpload.open(fileName, std::ios::trunc | std::ios::binary);
	delete fileName;
	return ;
}

bool Client::isThereFilename( int bodyType )
{
	char *crlfIndex;
	char *fileIndex;
	int i;

	if (!gotFileName)
	{
		if (bodyType == MULTIPART)
			crlfIndex = strstr(request, "\r\n\r\n");
		else if (bodyType == CHUNKED_MULTIPART)
			crlfIndex = strstr(request, "\r\n\r\n\r\n");
		if (crlfIndex == NULL)
			return false;
		else
		{
			fileIndex = strstr(request, "filename=");
			if (fileIndex != NULL && *(fileIndex + 10) != '\"')
			{
				fileIndex += 10;
				gotFileName = true;
				openFile(fileIndex);
			}
			else
				shouldSkip = true;
			if (bodyType == MULTIPART)
			{
				for (i = 0; request + i < crlfIndex + 4; i++);
				memmove(request, crlfIndex + 4, bytesRead - i + 1);
				bytesRead -= i;
			}
			else if (bodyType == CHUNKED_MULTIPART)
			{
				for (i = 0; request + i < crlfIndex + 6; i++);
				memmove(request, crlfIndex + 6, bytesRead - i + 1);
				bytesRead -= i;
			}
			// bytesToRead = giveDecimal(stringRequest);
			// stringRequest.erase(0, stringRequest.find('\r') + 2);
		}
	}
	return true;
}

char *Client::giveBody( char *limiter )
{
	int i;
	char *retString;

	for (i = 0; request + i != limiter; i++);
	retString = new char[i + 1];
	for (i = 0; request + i != limiter; i++)
		retString[i] = request[i];
	retString[i] = '\0';
	return retString;
}

char *Client::giveDelimiter( void )
{
	char *retString;
	int i;

	for (i = 0; request[i] != '\0' && request[i] != '\r'; i++);
	retString = new char[i + 1];
	for (i = 0; request[i] != '\0' && request[i] != '\r'; i++)
		retString[i] = request[i];
	retString[i] = '\0';
	return retString;
}

bool Client::isBoundary( char *ptr )
{
	int i;

	while (i < bytesRead && ptr[i] == boundary[i])
		i++;
	return i == boundary.size();
}

void Client::parseMultipartBody( void )
{
	int i;
	bool isFound;

	isFound = false;
	if (!isThereFilename(MULTIPART))
		return ;
	for (i = 0; i < bytesRead; i++)
	{
		if (request[i] == '\r' && i + 2 < bytesRead && request[i + 2] == '-')
			isFound = isBoundary(&request[i + 2]);
		if (isFound)
			break;
		if (!shouldSkip)
			fileToUpload << request[i];
	}
	if (!isFound)
	{
		memset(request, 0, bytesRead);
		bytesRead = 0;
		return ;
	}
	i += 2;
	bytesRead -= i;
	shouldSkip = false;
	gotFileName = false;
	fileToUpload.close();
	if (request[i + boundary.size()] == '-' && request[i + boundary.size() + 1] == '-')
	{
		finishedBody = true;
		return ;
	}
	memmove(request, &request[i], bytesRead + 1);
	parseMultipartBody();
}

// void Client::parseChunkedMultipart( void )
// {
// 	if (!isThereFilename(CHUNKED_MULTIPART))
// 		return ;

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

size_t Client::giveDecimal( const std::string &hexaString )
{
	std::stringstream ss;
	size_t ret;

	ss << std::hex << hexaString;
	ss >> ret;
	return ret;
}

void Client::parseChunkedBody( void )
{
	size_t i;

	i = 0;
	if (bytesToRead == 0)
	{
		for (; request[i] != '\r'; i++);
		bytesToRead = giveDecimal(std::string(request, request + i));
		memmove(request, &request[i + 2], (bytesRead - (i + 2)) + 1);
	}
	// BytesRead is the size of the request
	std::cout << "------------------" << std::endl;
	std::cout << "Size of the request : " << bytesRead << std::endl;
	std::cout << "To read : " << bytesToRead << std::endl;
	i = 0;
	for (; i < bytesToRead && i < bytesRead; i++)
		fileToUpload << request[i];
	std::cout << "i = " << i << std::endl;
	bytesToRead = bytesToRead - i;
	std::cout << "New bytes to read : " <<  bytesToRead << std::endl;
	std::cout << "------------------" << std::endl;
	// std::cout << request << std::endl;
	if (i == atoi(parsedRequest._headers["Content-Length"].c_str()))
	{
		finishedBody = true;
		fileToUpload.close();
	}
	else if (i == bytesRead)
	{
		memset(request, 0, MAX_RQ);
		bytesRead = 0;
	}
	else if (bytesToRead == 0)
	{
		std::cout << "||||||||||||||||||||||||||" << std::endl;
		std::cout << "i = " << i << std::endl;
		std::cout <<
		memmove(request, &request[i + 2], (bytesRead - (i + 2)) + 1);
		std::cout << request << std::endl;
		exit(0);
		parseChunkedBody();
	}
	else
	{
		std::cout << "=============================\n";
		std::cout << request[i] << std::endl;
		std::cout << "In else " << std::endl;
		std::cout << "Should read : " << bytesToRead << std::endl;
		std::cout << request << std::endl;
		exit(0);
	}
}

// void Client::parseChunkedBody( void )
// {
// 	int i, putInFile;

// 	i = 0;
// 	putInFile = 0;
// 	std::cout << "================================\n";
// 	std::cout << "Bytes to read = " << bytesToRead << std::endl;
// 	std::cout << "Bytes in total = " << bytesCounter << std::endl;
// 	std::cout << "Content Length = " << atoi(parsedRequest._headers["Content-Length"].c_str()) << std::endl;
// 	std::cout << request << std::endl;
// 	if (bytesToRead == 0)
// 	{
// 		for (; request[i] != '\r'; i++);
// 		bytesToRead = giveDecimal(std::string(request, request + i));
// 		i += 2;
// 	}
// 	for (; putInFile < bytesToRead && i < bytesRead; i++)
// 	{
// 		fileToUpload << request[i];
// 		putInFile++;
// 	}
// 	bytesCounter += putInFile;
// 	bytesToRead -= putInFile;
// 	// Need to change atoi
// 	if (bytesCounter == atoi(parsedRequest._headers["Content-Length"].c_str()))
// 	{
// 		finishedBody = true;
// 		fileToUpload.close();
// 	}
// 	else if (i == bytesRead)
// 	{
// 		memset(request, 0, MAX_RQ + 1);
// 		bytesRead = 0;
// 	}
// 	else
// 	{
// 		memmove(request, &request[i + 2], bytesRead - (i + 2) + 1);
// 		parseChunkedBody();
// 	}
// }

bool Client::isEndOfBody( void )
{
	std::string hexStr;
	size_t decimal;

	if (stringRequest.size() <= 9)
		return false;
	if (stringRequest[0] == '\r' && stringRequest[1] == '\n')
		hexStr = stringRequest.substr(2, stringRequest.find('\r', 2) - 2);
	decimal = giveDecimal(hexStr);
	if (decimal == 2 && stringRequest.substr(5, 4) == "\r\n\r\n")
	{
		stringRequest.erase(0, 9);
		return true;
	}
	else if (decimal == (boundary.size() + 6))
	{
		decimal = stringRequest.find('\r', 2);
		if (stringRequest.substr(decimal + 4, stringRequest.find('\r', decimal + 4) - (decimal + 4))
			== (boundary + "--"))
		{
			std::cout << "Finished Body" << std::endl;
			finishedBody = true;
		}
		return true;
	}
	return false;
}