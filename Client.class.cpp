#include "MainHeader.hpp"

Client::Client( void ) : _socket(0), bytesRead(0),\
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false), shouldReadBody(false), errString(), \
		finishedBody(false), gotFileName(false), shouldSkip(false), \
		bytesToRead(0), bytesCounter(0), contentLength(0), bodyType(0), \
		typeCheck(POLLIN) { }

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
		this->typeCheck = rhs.typeCheck;
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