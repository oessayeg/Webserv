#include "Webserver.hpp"

Webserver::Webserver( void ) : _socket(0) { }

Webserver::Webserver( const Webserver &rhs ) : _socket(rhs._socket) { }

Webserver &Webserver::operator=( const Webserver &rhs )
{
    this->_socket = rhs._socket;
    return *this;
}

Webserver::~Webserver( void ) { }

int Webserver::getSocket( void ) const
{
    return this->_socket;
}