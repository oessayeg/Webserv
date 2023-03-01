#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0), response(NULL), \
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isParsed(false) { }

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
		this->isParsed = rhs.isParsed;
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
void Client::readAndParse( void )
{
	int r;

	// Here I should check for a closed connection
	r = recv(_socket, request, MIN_TO_READ, 0);
	bytesRead += r;
	request[bytesRead] = '\0';
	stringRequest += request;
	if (strstr(request, "\r\n\r\n"))
		isParsed = true;
}