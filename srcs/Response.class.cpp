#include "../includes/Response.class.hpp"

Response::Response( void ) : _canBeSent(false), _shouldReadFromFile(false), \
	_isStatusSent(false), _fileSize(0), r(0) { }

Response::Response( const Response &rhs )
{
	*this = rhs;
}

Response &Response::operator=( const Response &rhs )
{
	_nameOfFile = rhs._nameOfFile;
	_response = rhs._response;
	_status = rhs._status;
	_canBeSent = rhs._canBeSent;
	_shouldReadFromFile = rhs._shouldReadFromFile;
	_isStatusSent = rhs._isStatusSent;
	_fileSize = rhs._fileSize;
	r = rhs.r;
	return *this;
}

Response::~Response( void ) { }

bool Response::getBool( void ) const
{
	return (this->_canBeSent);
}

bool Response::readFromFile( void ) const
{
	return (this->_shouldReadFromFile);
}

bool Response::getIsStatusSent( void ) const
{
	return (this->_isStatusSent);
}

void Response::setBool( bool isReady )
{
	this->_canBeSent = isReady;
}

void Response::setReadFromFile( bool shouldRead )
{
	this->_shouldReadFromFile = shouldRead;
}

void Response::setIsStatusSent( bool isItSent )
{
	this->_isStatusSent = isItSent;
}

void Response::setFileSize( size_t size )
{
	this->_fileSize = size;
}

void Response::sendResponse( int socket ) const
{
	send(socket, _response.c_str(), _response.size(), 0);
}

void Response::setResponse( const std::string &resp )
{
	this->_response = resp;
}

size_t Response::getFileSize( void ) const
{
	return (this->_fileSize);
}