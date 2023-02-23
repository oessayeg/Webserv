#include "Webserver.class.hpp"

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients() , _fds(NULL) { }

Webserver::Webserver( std::list < blocks > &rhs ) : _serverBlocks(rhs) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
    _pendingClients(rhs._pendingClients), _fds(rhs._fds) { }

Webserver::~Webserver( void ) { }

void Webserver::setServerBlocks( std::list < struct blocks > &list )
{
    this->_serverBlocks = list;
}