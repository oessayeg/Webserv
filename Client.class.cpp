#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0), response(NULL), \
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false), isRqLineParsed(false), \
		isHeaderParsed(false) { }

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
		// this->response = new char[strlen(rhs.response)];
		// *this->response = *rhs.response;
		this->correspondingBlock = rhs.correspondingBlock;
		this->isRead = rhs.isRead;
		this->isRqLineParsed = rhs.isRqLineParsed;
		this->isHeaderParsed = rhs.isHeaderParsed;
		*this->request = *rhs.request;
		this->parsedRequest = rhs.parsedRequest;
		this->clientStruct = new struct sockaddr_in;
		*this->clientStruct = *rhs.clientStruct;
	}
	return *this;
}

Client::~Client( void )
{
	// delete this->response;
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
	this->clientResponse.setBool(true);

	if (!parsedRequest.hasAllowedChars())
		this->clientResponse.setResponse("HTTP/1.1 400 Bad Request\r\n\r\n<h1>Bad Request !</h1>");
	else if (!parsedRequest.hasGoodSize())
		this->clientResponse.setResponse("HTTP/1.1 414 Request-URI too long\r\n\r\n<h1>Request uri too long !</h1>");
	else if (!parsedRequest.isSupported())
		this->clientResponse.setResponse("HTTP/1.1 405 Method Not Allowed\r\n\r\n<h1>This method is not allowed !</h1>");
	else if (!parsedRequest.isGoodVersion())
		this->clientResponse.setResponse("HTTP/1.1 505 Version Not Supported\r\n\r\n</h1>This version is not supported !</h1>");
	else
		this->clientResponse.setBool(false);
}

void Client::checkHeaders( void )
{
	if (parsedRequest._headers.find("Content-Length") != parsedRequest._headers.end()
		&& atoi(parsedRequest._headers["Content-Length"].c_str()) > correspondingBlock->port)
	{
		clientResponse.setResponse("HTTP/1.1 413 Content Too Large\r\n\r\n<h1>Body content too large !</h1>");
		clientResponse.setBool(true);
	}
	else if (parsedRequest._headers.find("Transfer-Encoding") != parsedRequest._headers.end()
		&& parsedRequest._headers["Transfer-Encoding"] != "chunked")
	{
		clientResponse.setResponse("HTTP/1.1 501 Not Implement\r\n\r\n<h1>Not implemented !</h1>");
		clientResponse.setBool(true);
	}
	else if (parsedRequest._method == "POST" && parsedRequest._headers.find("Content-Length")
		== parsedRequest._headers.end() && parsedRequest._headers.find("Transfer-Encoding")
		== parsedRequest._headers.end())
	{
		clientResponse.setResponse("HTTP/1.1 400 Bad Request\r\n\r\n<h1>Bad Request !</h1>");
		clientResponse.setBool(true);
	}
}