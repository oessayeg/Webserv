#include "../includes/MainHeader.hpp"

Client::Client( void ) : _socket(0),\
		clientStruct(new struct sockaddr_in), bodyType(0), isRead(false), \
		isRqLineParsed(false), isHeaderParsed(false), shouldReadBody(false), \
		finishedBody(false), gotFileName(false), shouldSkip(false), isConnected(true), \
		isThereCgi(false), typeCheck(POLLIN), bytesRead(0), bytesToRead(0), bytesCounter(0), \
		contentLength(0), correspondingBlock(NULL), errString(), parsedRequest() { }

Client::Client( const Client &rhs )
{
	*this = rhs;
}

Client &Client::operator=( const Client &rhs )
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->clientStruct = new struct sockaddr_in;
		if (this->clientStruct != NULL)
			*this->clientStruct = *rhs.clientStruct;
		*this->request = *rhs.request;
		this->bodyType = rhs.bodyType;
		this->isRead = rhs.isRead;
		this->isRqLineParsed = rhs.isRqLineParsed;
		this->isHeaderParsed = rhs.isHeaderParsed;
		this->shouldReadBody = rhs.shouldReadBody;
		this->finishedBody = rhs.finishedBody;
		this->gotFileName = rhs.gotFileName;
		this->shouldSkip = rhs.shouldSkip;
        this->isConnected = rhs.isConnected;
		this->isThereCgi = rhs.isThereCgi;
		this->typeCheck = rhs.typeCheck;
		this->bytesRead = rhs.bytesRead;
		this->bytesToRead = rhs.bytesToRead;
		this->bytesCounter = rhs.bytesCounter;
		this->contentLength = rhs.contentLength;
		this->stringRequest = rhs.stringRequest;
		this->boundary = rhs.boundary;
		this->filePath = rhs.filePath;
		this->nameForCgi = rhs.nameForCgi;
		this->currentList = rhs.currentList;
		this->correspondingBlock = new Serverblock;
		if (rhs.correspondingBlock != NULL)
			*this->correspondingBlock = *rhs.correspondingBlock;
		this->clientResponse = rhs.clientResponse;
		this->errString = rhs.errString;
		this->parsedRequest = rhs.parsedRequest;
	}
	return *this;
}

Client::~Client( void )
{
	delete this->clientStruct;
	if (correspondingBlock)
		delete correspondingBlock;
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
	if (!parsedRequest.hasAllowedChars())
		Utils::setErrorResponse(400, "HTTP/1.1 400 Bad Request\r\n", "Bad Request", *this);
	else if (!parsedRequest.hasGoodSize())
		Utils::setErrorResponse(414, "HTTP/1.1 414 Request-URI too long\r\n", "Uri Too Long", *this);
	else if (!parsedRequest.isSupported())
		Utils::setErrorResponse(405, "HTTP/1.1 405 Method Not Allowed\r\n", "Method Not Allowed", *this);
	else if (!parsedRequest.isGoodVersion())
		Utils::setErrorResponse(505, "HTTP/1.1 505 Version Not Supported\r\n", "Version Not Supported", *this);
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
	if (contentLength > correspondingBlock->get_body_size() * 1000000)
		Utils::setErrorResponse(413, "HTTP/1.1 413 Request Entity Too Large\r\n", "Request Entity Too Large", *this);
	else if ((!transferEnc.empty() && transferEnc != "chunked")
		|| (transferEnc == "chunked" && contentType.find("multipart") != std::string::npos))
		Utils::setErrorResponse(501, "HTTP/1.1 501 Not Implemented\r\n", "Not Implemented", *this);
	else if (parsedRequest._method == "POST" && contentLength == 0 && transferEnc.empty())
		Utils::setErrorResponse(400, "HTTP/1.1 400 Bad Request\r\n", "Bad Request", *this);
	if (clientResponse.getBool() || parsedRequest._method != "POST")
		return ;
	// This part will set the correct reading mode for the body
	this->setType(transferEnc, contentType);
}

// This function forms the whole response when an error happens
std::string Client::formError( int statusCode, const std::string &statusLine, const std::string &msgInBody )
{
	std::map< int, std::string >::iterator b;
	std::string returnString, fileInString;
	std::ifstream errorFile;
	std::stringstream s;

	b = correspondingBlock->_error_page.find(statusCode);
	if (b != correspondingBlock->_error_page.end())
		errorFile.open(b->second);
	if (b != correspondingBlock->_error_page.end() && !errorFile.is_open())
	{
		returnString = "HTTP/1.1 500 Internal Server Error\r\n";
		errString.setErrorFile(500, "Internal Server Error");
		s << errString.getFileInString().size();
		returnString += "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n\r\n";
		return (returnString + errString.getFileInString());
	}
	else if (b != correspondingBlock->_error_page.end() && errorFile.is_open())
	{
		fileInString = std::string((std::istreambuf_iterator<char>(errorFile)), (std::istreambuf_iterator<char>()));
		s << fileInString.size();
		returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n\r\n";
		errorFile.close();
		return returnString + fileInString;
	}
	errString.setErrorFile(statusCode, msgInBody);
	s << errString.getFileInString().size();
	returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str();
	returnString += "\r\n\r\n";
	return (returnString + errString.getFileInString());
}

bool isAccepted( std::string method, std::list< std::string > list )
{
	std::list< std::string >::iterator it1;

	it1 = list.begin();
	for (; it1 != list.end(); it1++)
		if (method == *it1)
			return true;
	return false;
}

void Client::setType( const std::string &transferEnc, const std::string &contentType )
{
	if (!this->isLocationFormedWell(transferEnc))
		return ;
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

bool Client::isLocationFormedWell( const std::string &transferEnc )
{
	std::list< Location >::iterator currentList;
	
	if (this->correspondingBlock->_serverNames.size() > 0 && !Utils::serverNameMatches(this->parsedRequest._headers["Host"], this->correspondingBlock))
	{
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", *this);
		return false;
	}
	currentList = correspondingBlock->ifUriMatchLocationBlock(correspondingBlock->_location, parsedRequest._uri);
	if (currentList == correspondingBlock->_location.end()
		|| (currentList->_supportUpload && (!currentList->checkIfPathExist(currentList->_upload_dir) || !currentList->ifRequestUriIsFolder(currentList->_upload_dir))))
	{
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "Not Found", *this);
		return false;
	}
	else if (!isAccepted("POST", currentList->_accept_list))
	{
		Utils::setErrorResponse(405, "HTTP/1.1 405 Not Allowed\r\n", "Method Not Allowed", *this);
		return false;
	}
	else if (currentList->get_isThereRedirection())
	{
		Utils::setGoodResponse("HTTP/1.1 301 Moved Permanently\r\nLocation: " + currentList->_redirection[1] + "\r\n" + "Content-Length: 0\r\n\r\n", *this);
		return false;
	}
	else if ((currentList->_currentRoot.back() == '/' && !currentList->get_indexes_location().size()
		&& !currentList->_supportUpload) || (!currentList->get_cgi() && !currentList->_supportUpload))
	{
		Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden\r\n", "Forbidden", *this);
		return false;
	}
	else if (currentList->get_cgi() && !currentList->_supportUpload && contentLength > 0)
	{
		this->isThereCgi = true;
		this->shouldReadBody = true;
		this->bodyType = OTHER;
		if (transferEnc == "chunked")
			this->bodyType = CHUNKED;
		this->filePath = "/tmp/";
		return false;
	}
	this->filePath = currentList->_upload_dir;
	return true;
}
