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
	std::list< blocks >::iterator b;
	struct sockaddr_in socketNeeds;
	int sock;
	int tmp;

	sock = 0;
	tmp = 1;
	for (b = this->_serverBlocks.begin(); b != this->_serverBlocks.end(); b++)
	{
		memset(&socketNeeds, 0, sizeof(sockaddr_in)); // Should not forget to change memset (not allowed)
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			throw "Socket function failed"; // Need the right exception
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) == -1)
			throw "Setsockopt function failed"; // Need the right exception
		socketNeeds.sin_port = htons(b->port);
		socketNeeds.sin_family = PF_INET;
		socketNeeds.sin_addr.s_addr = b->ip; // The ip should be in 'long' format
		if (bind(sock, (struct sockaddr *)&socketNeeds, sizeof(sockaddr_in)) == -1)
			throw "Bind function failed"; // Need the right exception
		if (listen(sock, 0) == -1)
			throw "Listen function failed"; // Need the right exception
		this->_listeningSockets.push_back(sock);
	}
}

void Webserver::setReadyFds( void )
{
	int sumOfAllSockets, index;
	std::list< int >::iterator b1;
	std::list< clients >::iterator b2;

	sumOfAllSockets = this->_listeningSockets.size() + this->_pendingClients.size();
	this->_fds = new pollfd[sumOfAllSockets];

	index = 0;
	for (b1 = this->_listeningSockets.begin(); b1 != this->_listeningSockets.end(); b1++)
	{
		this->_fds[index].fd = *b1;
		this->_fds[index].events = POLLRDNORM;
		index++;
	}
	for (b2 = this->_pendingClients.begin(); b2 != this->_pendingClients.end(); b2++)
	{
		this->_fds[index].fd = b2->fd;
		this->_fds[index].events = POLLRDNORM | POLLWRNORM;
		index++;
	}
	if (poll(this->_fds, sumOfAllSockets, -1) < 0)
		throw "Poll function failed"; // Need to change exception
}

void Webserver::readAndRespond( void )
{
	std::list< int >::iterator b1;
	std::list< clients >::iterator b2;
	socklen_t size;
	int index, oldClientsSize;

	index = 0;
	oldClientsSize = this->_pendingClients.size();
	size = sizeof(struct sockaddr_in);
	for (b1 = this->_listeningSockets.begin(); b1 != this->_listeningSockets.end(); b1++)
	{
		if (this->_fds[index].revents == POLLRDNORM)
		{
			clients newClient;

			newClient.bytesRead = 0;
			newClient.fd = accept(this->_fds[index].fd, (struct sockaddr *)&newClient.clientStruct, &size);
			this->_pendingClients.push_back(newClient);
		}
		index++;
	}
	b2 = this->_pendingClients.begin();
	for (int x = 0; x < oldClientsSize; x++)
	{
		if (this->_fds[index].fd == b2->fd && this->_fds[index].revents & POLLRDNORM)
			this->_readRequest(b2);
		else if (this->_fds[index].fd == b2->fd && this->_fds[index].revents & POLLWRNORM)
			this->_sendResponse(b2);
		b2++;
		index++;
	}
	delete this->_fds;
}

void Webserver::_readRequest( std::list< clients >::iterator client )
{
	std::cout << "Should read request from : " << client->fd << std::endl;
	exit(0);
}

void Webserver::_sendResponse( std::list< clients >::iterator client )
{
	std::cout << "Should send response to : " << client->fd << std::endl;
	exit(0);
}