#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0),\
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false), shouldReadBody(false), errString(), \
		finishedBody(false), gotFileName(false), shouldSkip(false), \
		bytesToRead(0) { }

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

bool Client::isThereFilename( int bodyType )
{
	int crlfIndex;
	int fileIndex;

	if (!gotFileName)
	{
		if (bodyType == MULTIPART)
			crlfIndex = stringRequest.find("\r\n\r\n");
		else if (bodyType == CHUNKED_MULTIPART)
			crlfIndex = stringRequest.find("\r\n\r\n\r\n");
		if (crlfIndex == std::string::npos)
			return false;
		else
		{
			fileIndex = stringRequest.find("filename=");
			// Should check if filename is empty
			if (fileIndex != std::string::npos)
			{
				fileIndex += 10;
				gotFileName = true;
				fileToUpload.open(stringRequest.substr(fileIndex, stringRequest.find('\"', fileIndex) - fileIndex), std::ios::trunc);
			}
			else
				shouldSkip = true;
			if (bodyType == MULTIPART)
				stringRequest.erase(0, crlfIndex + 4);
			else if (bodyType == CHUNKED_MULTIPART)
				stringRequest.erase(0, crlfIndex + 6);
			// bytesToRead = giveDecimal(stringRequest);
			// stringRequest.erase(0, stringRequest.find('\r') + 2);
		}
	}
	return true;
}

// Should consider moving the check functions/parse functions to Webserv class
void Client::parseMultipartBody( void )
{
	std::string lineToAdd;
	int endOfLineIndex;

	if (!isThereFilename(MULTIPART))
		return ;
	endOfLineIndex = stringRequest.find('\n');
	lineToAdd = stringRequest.substr(0, endOfLineIndex + 1);
	while (endOfLineIndex != std::string::npos)
	{
		// Should not forget to check if it is the end of the body with the --
		// Here should double check for windows files
		if (endOfLineIndex > 0 && lineToAdd[endOfLineIndex - 1] == '\r')
		{
			stringRequest.erase(0, endOfLineIndex + 1);
			fileToUpload.close();
			gotFileName = false;
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

void Client::parseChunkedMultipart( void )
{
	int endOfBody;

	if (!this->isThereFilename(CHUNKED_MULTIPART))
		return ;
	if (bytesToRead == 0 && stringRequest.find("\r\n") == std::string::npos)
		return ;
	else if (bytesToRead == 0 && isEndOfBody())
	{
		std::cout << "---------Here--------" << std::endl;
		gotFileName = false;
		shouldSkip = false;
		fileToUpload.close();
	}
	else if (bytesToRead == 0 && gotFileName)
	{
		bytesToRead = giveDecimal(stringRequest);
		stringRequest.erase(0, stringRequest.find('\n') + 1);
	}
	if (bytesToRead > 0 && stringRequest.size() > 0 && stringRequest.size() < bytesToRead)
	{
		if (!shouldSkip)
			fileToUpload << stringRequest;
		bytesToRead -= stringRequest.size();
		return ;
	}
	else if (bytesToRead > 0 && stringRequest.size() > 0 && stringRequest.size() >= bytesToRead)
	{
		// In case the size of the string is bigger that the bytes to read
		stringRequest.erase(0, bytesToRead);
		bytesToRead = 0;
		if (!shouldSkip)
			fileToUpload << stringRequest.substr(0, bytesToRead);
	}
	parseChunkedMultipart();
}

size_t Client::giveDecimal( std::string &hexaString )
{
	std::stringstream ss;
	size_t ret;

	ss << std::hex << hexaString.substr(0, hexaString.find('\r'));
	ss >> ret;
	return ret;
}

bool Client::isEndOfBody( void )
{
	std::string hexStr;

	if (stringRequest.size() <= 9)
		return false;
	if (stringRequest[0] == '\r' && stringRequest[1] == '\n')
		hexStr = stringRequest.substr(2, stringRequest.find('\r', 2) - 2);
	if (giveDecimal(hexStr) == 2 && stringRequest.substr(5, 4) == "\r\n\r\n")
	{
		stringRequest.erase(0, 9);
		return true;
	}
	return false;
}