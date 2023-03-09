#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0),\
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false), shouldReadBody(false), errString(), \
		finishedBody(false), gotFileName(false), shouldSkip(false) { }

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
		this->correspondingBlock = rhs.correspondingBlock;
		this->isRead = rhs.isRead;
		this->isRqLineParsed = rhs.isRqLineParsed;
		this->isHeaderParsed = rhs.isHeaderParsed;
		this->shouldReadBody = rhs.shouldReadBody;
		this->finishedBody = rhs.finishedBody;
		this->gotFileName = rhs.gotFileName;
		this->shouldSkip = rhs.shouldSkip;
		this->boundary = rhs.boundary;
		this->body = rhs.body;
		*this->request = *rhs.request;
		this->parsedRequest = rhs.parsedRequest;
		this->clientStruct = new struct sockaddr_in;
		*this->clientStruct = *rhs.clientStruct;
		this->errString = rhs.errString;
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

// Should consider moving the check functions/parse functions to Webserv class
void Client::parseMultipartBody( void )
{
	std::string lineToAdd;
	int endOfLineIndex;
	int fileIndex;
	int crlfIndex;

	if (!gotFileName)
	{
		crlfIndex = stringRequest.find("\r\n\r\n");
		if (crlfIndex == std::string::npos)
			return ;
		else
		{
			fileIndex = stringRequest.find("filename=");
			if (fileIndex != std::string::npos)
			{
				fileIndex += 10;
				gotFileName = true;
				fileToUpload.open(stringRequest.substr(fileIndex, stringRequest.find('\"', fileIndex) - fileIndex), std::ios::trunc);
			}
			else
				shouldSkip = true;
			stringRequest.erase(0, crlfIndex + 4);
		}
	}
	endOfLineIndex = stringRequest.find('\n');
	lineToAdd = stringRequest.substr(0, endOfLineIndex + 1);
	while (endOfLineIndex != std::string::npos)
	{
		// Should not forget to check if it is the end of the body with the --
		// Here should double check for windows files
		if (endOfLineIndex > 0 && lineToAdd[endOfLineIndex - 1] == '\r')
		{
			stringRequest.erase(0, endOfLineIndex + 1);
			gotFileName = false;
			fileToUpload.close();
			shouldSkip = false;
			break;
		}
		stringRequest.erase(0, endOfLineIndex + 1);
		if (!shouldSkip)
			fileToUpload << lineToAdd;
		endOfLineIndex = stringRequest.find('\n');
		lineToAdd = stringRequest.substr(0, endOfLineIndex + 1);
	}
	if (endOfLineIndex == std::string::npos)
		return ;
	if (stringRequest.substr(0, stringRequest.find('\r')) == boundary + "--")
	{
		finishedBody = true;
		return ;
	}
	parseMultipartBody();
}