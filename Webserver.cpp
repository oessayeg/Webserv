#include "Webserver.hpp"

Webserver::Webserver( void ) : _socket(0) { }

Webserver::Webserver( const Webserver &rhs ) : _socket(rhs._socket) { }

Webserver &Webserver::operator=( const Webserver &rhs )
{
    this->_socket = rhs._socket;
    return *this;
}

Webserver::Webserver( const int portToUse ) : _port(portToUse) { }

Webserver::~Webserver( void ) { }

int Webserver::getSocket( void ) const
{
    return this->_socket;
}

void Webserver::createSocket( void )
{
    this->_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
        throw std::bad_exception();
}