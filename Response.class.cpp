#include "Response.class.hpp"

Response::Response( void ) : _response(), _canBeSent(false), _nameOfFile(), \
	_shouldReadFromFile(false), _isStatusSent(false), r(0) { }

Response::Response( const Response &rhs ) { *this = rhs; }

Response &Response::operator=( const Response &rhs )
{
	_nameOfFile = rhs._nameOfFile;
	_response = rhs._response;
	_canBeSent = rhs._canBeSent;
	_shouldReadFromFile = rhs._shouldReadFromFile;
	_isStatusSent = rhs._isStatusSent;
	this->r = rhs.r;
	return *this;
}

Response::~Response( void ) { }

bool Response::getBool( void ) const
{
	return _canBeSent;
}

void Response::sendResponse( int socket ) const
{
	send(socket, _response.c_str(), _response.size(), 0);
}

void Response::setResponse( const std::string &resp )
{
	_response = resp;
}

void Response::setBool( bool isReady )
{
	this->_canBeSent = isReady;
}