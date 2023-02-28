#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), _bytesRead(0), _response(NULL), \
		_clientStruct(new struct sockaddr_in), _parsedRequest(), \
		correspondingBlock(NULL) { }

Client::Client( const Client &rhs )
{
	*this = rhs;
}

Client &Client::operator=( const Client &rhs )
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_bytesRead = rhs._bytesRead;
		// this->_response = new char[strlen(rhs._response)];
		// *this->_response = *rhs._response;
		*this->_request = *rhs._request;
		this->_parsedRequest = rhs._parsedRequest;
		this->_clientStruct = new struct sockaddr_in;
		*this->_clientStruct = *rhs._clientStruct;
	}
	return *this;
}

Client::~Client( void )
{
	// delete this->_response;
	delete this->_clientStruct;
}

void Client::setSocket( int s )
{
	this->_socket = s;
}

int Client::getSocket( void ) const
{
	return (_socket);
}

struct sockaddr_in *Client::getClientStruct( void )
{
	return (_clientStruct);
}