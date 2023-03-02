#include "MainHeader.hpp"

// need to allocate 'response'
Client::Client( void ) : _socket(0), bytesRead(0), response(NULL), \
		clientStruct(new struct sockaddr_in), parsedRequest(), \
		correspondingBlock(NULL), isRead(false) { }

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
		this->isRead = rhs.isRead;
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

void Client::read( void )
{
	char *ptrToEnd;
	int r;

	// Here I should check for a closed connection or a fail from recv
	r = recv(_socket, request, MIN_TO_READ, 0);

	// Here I should check if the length is equal to the maximum one
	bytesRead += r;
	request[bytesRead] = '\0';
	stringRequest += request;
	if (stringRequest.find("\r\n\r\n") != std::string::npos)
		isRead = true;
}

void Client::parseRequest( void )
{
	int i1, i2;

	if (!isRead)
		return ;
	i1 = stringRequest.find(' ');
	parsedRequest.setMethod(stringRequest.substr(0, i1));
	i2 = stringRequest.find(' ', i1 + 1);
	parsedRequest.setUri(stringRequest.substr(i1 + 1, i2 - i1 - 1));
	i1 = stringRequest.find('\r', i2 + 1);
	parsedRequest.setVersion(stringRequest.substr(i2 + 1, i1 - i2 - 1));

	std::cout << "Method : " << parsedRequest._method << std::endl;
	std::cout << "Uri : " << parsedRequest._uri << std::endl;
	std::cout << "Version : " << parsedRequest._version << std::endl << std::endl;
}