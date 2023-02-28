#include "Webserver.class.hpp"

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients(), _listeningSockets(),\
	_fdToCheck(NULL) { }

Webserver::Webserver( std::list < Blocks > &rhs ) : _serverBlocks(rhs), _pendingClients(),\
	_listeningSockets(), _fdToCheck(NULL) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
	_pendingClients(rhs._pendingClients), _listeningSockets(rhs._listeningSockets), \
	_fdToCheck(rhs._fdToCheck) { }

Webserver::~Webserver( void ) { }

void Webserver::setServerBlocks( std::list < Blocks > list )
{
	this->_serverBlocks = list;
}

void Webserver::createSockets( void )
{
	std::list< Blocks >::iterator b;
	int sock;
	int tmp;

	sock = 0;
	tmp = 1;
	// Need to change the exceptions thrown
	std::cout << "Creating the listening sockets : ";
	for (b = this->_serverBlocks.begin(); b != this->_serverBlocks.end(); b++)
	{
		memset(&b->socketNeeds, 0, sizeof(sockaddr_in)); // Should not forget to change memset (not allowed)
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1)
			throw "Socket function failed";
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)) == -1)
			throw "Setsockopt function failed";
		b->socketNeeds.sin_port = htons(b->port);
		b->socketNeeds.sin_family = PF_INET;
		b->socketNeeds.sin_addr.s_addr = b->ip;
		if (bind(sock, (struct sockaddr *)&b->socketNeeds, sizeof(sockaddr_in)) == -1)
			throw "Bind function failed";
		if (listen(sock, 0) == -1)
			throw "Listen function failed";
		std::cout << sock << " ";
		this->_listeningSockets.push_back(sock);
	}
	std::cout << std::endl;
}

void Webserver::setReadyFds( void )
{
	std::list< Client >::iterator cIter;
	std::list< int >::iterator sIter;
	int i;

	i = 0;
	_fdToCheck = new pollfd[_listeningSockets.size() + _pendingClients.size()];
	std::cout << "Sockets : ";
	for (sIter = _listeningSockets.begin(); sIter != _listeningSockets.end(); sIter++)
	{
		std::cout << *sIter << " ";
		_fdToCheck[i].fd = *sIter;
		_fdToCheck[i].events = POLLRDNORM;
		i++;
	}
	std::cout << ", clients : ";
	for (cIter = _pendingClients.begin(); cIter != _pendingClients.end(); cIter++)
	{
		std::cout << cIter->getSocket() << " ";
		_fdToCheck[i].fd = cIter->getSocket();
		_fdToCheck[i].events = POLLRDNORM | POLLWRNORM;
		i++;
	}
	std::cout << std::endl;
}

void Webserver::readAndRespond( void )
{
	std::list< Client >::iterator b;
	int sizeOfSocketsAndClients;
	int nbFds;
	int i;

	sizeOfSocketsAndClients = this->_listeningSockets.size() + this->_pendingClients.size();
	nbFds = poll(this->_fdToCheck, sizeOfSocketsAndClients, -1);
	this->_acceptNewClients();
	b = _pendingClients.begin();
	for (i = _listeningSockets.size(); i < sizeOfSocketsAndClients; i++)
	{
		if (_fdToCheck[i].events & POLLRDNORM)
		{
			std::cout << "Should read from fd : " << _fdToCheck[i].fd << " ";
			// Here I should read the request 
		}
		else if (_fdToCheck[i].events & POLLWRNORM)
		{
			std::cout << "Should write in fd : " << _fdToCheck[i].fd << " ";
			// Here I should send the response
		}
		
		i++;
	}
	std::cout << std::endl;
}

void Webserver::_acceptNewClients( void )
{
	std::list< int >::iterator sockIter;
	std::list< Blocks >::iterator blIter;
	socklen_t sizeOfSockaddr_in;
	int newFd;
	int i;

	i = 0;
	newFd = 0;
	sizeOfSockaddr_in = sizeof(struct sockaddr_in);
	sockIter = _listeningSockets.begin();
	blIter = _serverBlocks.begin();
	for (; sockIter != _listeningSockets.end(); sockIter++)
	{
		if (_fdToCheck[i].revents == POLLRDNORM)
		{
			Client newClient;

			newFd = accept(*sockIter, (struct sockaddr *)newClient.getClientStruct(), &sizeOfSockaddr_in);
			if (newFd == -1)
				throw "Accept function failed";
			newClient.correspondingBlock = &(*blIter);
			newClient.setSocket(newFd);
			_pendingClients.push_back(newClient);
		}
		i++;
		blIter++;
	}
}

// bool Webserver::_parseRequest( std::list< clients * >::iterator client )
// {
// 	std::string req((*client)->request);
// 	int index1, index2;

// 	index1 = 0;
// 	if (!(*client)->parsedRequest.isRequestLineParsed() && req.find("\r\n") >= 0)
// 	{
// 		index1 = req.find(" ", 0);
// 		(*client)->parsedRequest.setMethod(req.substr(0, index1));
// 		if (!(*client)->parsedRequest.isSupported())
// 		{
// 			// send405(client);
// 			return (false);
// 		}
// 		index2 = req.find(" ", index1 + 1);
// 		(*client)->parsedRequest.setUri(req.substr(index1 + 1, index2 - index1 - 1));
// 		if (!(*client)->parsedRequest.hasGoodSize())
// 		{
// 			// send414(client);
// 			return (false);
// 		}
// 		else if (!(*client)->parsedRequest.hasAllowedChars())
// 		{
// 			// send400((*client));
// 			return (false);
// 		}
// 		index1 = req.find("\r\n", index2 + 1);
// 		(*client)->parsedRequest.setVersion(req.substr(index2 + 1, index1 - index2 - 1));
// 		if (!(*client)->parsedRequest.isGoodVersion())
// 		{
// 			// send505(client);
// 			return (false);
// 		}
// 	}
// 	return (true);
// }

// void Webserver::send405( std::list< clients >::iterator client )
// {
// 	send(client->fd, "HTTP/1.1 405 Method Not Allowed\r\n\r\n<h1>This method is not allowed !</h1>", 73, 0);
// 	close(client->fd);
// 	this->_pendingClients.erase(client);
// }

// void Webserver::send414( std::list< clients >::iterator client )
// {
// 	send(client->fd, "HTTP/1.1 414 Request-URI too long\r\n\r\n<h1>The request uri is too long !</h1>", 76, 0);
// 	close(client->fd);
// 	this->_pendingClients.erase(client);
// }

// void Webserver::send400( std::list< clients >::iterator client )
// {
// 	send(client->fd, "HTTP/1.1 400 Bad Request\r\n\r\n<h1>Bad Request !</h1>", 51, 0);
// 	close(client->fd);
// 	this->_pendingClients.erase(client);
// }

// void Webserver::send505( std::list< clients >::iterator client )
// {
// 	send(client->fd, "HTTP/1.1 501 Http Version Not Supported\r\n\r\n<h1>Http Version Not Supported !</h1>", 81, 0);
// 	close(client->fd);
// 	this->_pendingClients.erase(client);
// }