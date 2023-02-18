#include "Webserver.hpp"

// Network byte order is big-endian, so htons make sure that our nbr is big-endian \
// if our machine stores nbrs in little endian format

Webserver::Webserver( void ) : _socket(0), _port(0), _serverAddress(NULL), _clientAddress(NULL), _sockaddLen(0)  { }

Webserver::Webserver( const Webserver &rhs ) : _socket(rhs._socket), _port(rhs._port)
{
    this->_serverAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    this->_clientAddress = new sockadd;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
}

Webserver &Webserver::operator=( const Webserver &rhs )
{
    this->_socket = rhs._socket;
    this->_port = rhs._port;
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
    return *this;
}

Webserver::Webserver( const int portToUse ) : _port(portToUse)
{
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    this->_serverAddress->sin_family = PF_INET;
    this->_serverAddress->sin_addr.s_addr = 0;
    this->_serverAddress->sin_port = htons(this->_port);
    this->_sockaddLen = sizeof(_serverAddress);
}

Webserver::~Webserver( void )
{
    delete _serverAddress;
    delete _clientAddress;
}

int Webserver::getSocket( void ) const
{
    return this->_socket;
}

void Webserver::createSocket( void )
{
    this->_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::bad_exception();
    close(this->_socket);
}