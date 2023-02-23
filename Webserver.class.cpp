#include "Webserver.class.hpp"

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients(), _listeningSockets(),\
	_fds(NULL) { }

Webserver::Webserver( std::list < blocks > &rhs ) : _serverBlocks(rhs), _pendingClients(),\
	_listeningSockets(), _fds(NULL) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
	_pendingClients(rhs._pendingClients), _listeningSockets(rhs._listeningSockets), \
	_fds(rhs._fds) { }

Webserver::~Webserver( void ) { }

void Webserver::setServerBlocks( std::list < blocks > list )
{
	this->_serverBlocks = list;
}

void Webserver::createSockets( void )
{
	std::list< blocks >::iterator begin, end;
	struct sockaddr_in socketNeeds;
	int sock;
	int tmp;

	sock = 0;
	tmp = 1;
	begin = this->_serverBlocks.begin();
	end = this->_serverBlocks.end(); 
	for (; begin != end; begin++)
	{
		memset(&socketNeeds, 0, sizeof(sockaddr_in)); // Should not forget to change memset (not allowed)
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			throw "Socket function failed"; // Need the right exception
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) == -1)
			throw "Setsockopt function failed"; // Need the right exception
		socketNeeds.sin_port = htons(begin->port);
		socketNeeds.sin_family = PF_INET;
		socketNeeds.sin_addr.s_addr = begin->ip; // The ip should be in 'long' format
		if (bind(sock, (struct sockaddr *)&socketNeeds, sizeof(sockaddr_in)) == -1)
			throw "Bind function failed"; // Need the right exception
		if (listen(sock, 0) == -1)
			throw "Listen function failed"; // Need the right exception
		this->_listeningSockets.push_back(sock);
	}
}