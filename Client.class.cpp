#include "MainHeader.hpp"

Client::Client( void ) : _response(NULL), _bytesRead(0), _socket(0), _parsedRequest() { }

Client::Client( const Client &rhs )
{
	*this = rhs;
}

Client &Client::operator=( const Client &rhs )
{
	if (this != &rhs)
	{
		this->_port = rhs._port;
		this->_socket = rhs._socket;
		this->_bytesRead = rhs._bytesRead;
		this->_response = new char[strlen(rhs._response)];
		*this->_response = *rhs._response;
		*this->_request = *rhs._request;
		this->_parsedRequest = rhs._parsedRequest;
		this->_clientStruct = new struct sockaddr_in;
	}
	return *this;
}

Client::~Client( void )
{
	// delete this->_response;
	delete this->_clientStruct;
}