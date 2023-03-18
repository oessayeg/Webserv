#include "Webserver.class.hpp"

// Should not forget to change map to hashmap for efficiency

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
	for (b = this->_serverBlocks.begin(); b != this->_serverBlocks.end(); b++)
	{
		memset(&b->socketNeeds, 0, sizeof(sockaddr_in));
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
		this->_listeningSockets.push_back(sock);
	}
}

void Webserver::setReadyFds( void )
{
	std::list< Client >::iterator cIter;
	std::list< int >::iterator sIter;
	int i;

	i = 0;
	_fdToCheck = new pollfd[_listeningSockets.size() + _pendingClients.size()];
	for (sIter = _listeningSockets.begin(); sIter != _listeningSockets.end(); sIter++)
	{
		_fdToCheck[i].fd = *sIter;
		_fdToCheck[i].events = POLLIN;
		_fdToCheck[i].revents = 0;
		i++;
	}
	for (cIter = _pendingClients.begin(); cIter != _pendingClients.end(); cIter++)
	{
		_fdToCheck[i].fd = cIter->getSocket();
		_fdToCheck[i].events = cIter->typeCheck;
		_fdToCheck[i].revents = 0;
		i++;
	}
}

void Webserver::readAndRespond( void )
{
	std::list< Client >::iterator b;
	int sizeOfSocketsAndClients;
	int nbFds;
	int i;
	bool increment;

	increment = true;
	sizeOfSocketsAndClients = this->_listeningSockets.size() + this->_pendingClients.size();
	// Need to check for nbFds when iterating through fds for optimization or for an error
	nbFds = poll(_fdToCheck, sizeOfSocketsAndClients, -1);
	this->_acceptNewClients();
	b = _pendingClients.begin();
	for (i = _listeningSockets.size(); i < sizeOfSocketsAndClients; i++)
	{
		if (_fdToCheck[i].revents & POLLIN)
			this->_readAndParse(*b);
		if (!b->isConnected)
			_dropClient(b, &increment, 0);
		else if (b->clientResponse.getBool() && _fdToCheck[i].revents & POLLOUT)
			_dropClient(b, &increment, 1);
		if (increment)
			b++;
		increment = true;
	}
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
		if (_fdToCheck[i].revents & POLLIN)
		{
			Client newClient;

			newFd = accept(*sockIter, (struct sockaddr *)newClient.clientStruct, &sizeOfSockaddr_in);
			if (newFd == -1)
				throw "Accept function failed";
			fcntl(newFd, F_SETFL, O_NONBLOCK);
			newClient.correspondingBlock = &(*blIter);
			newClient.setSocket(newFd);
			_pendingClients.push_back(newClient);
		}
		i++;
		blIter++;
	}
}

void Webserver::_readRequest( Client &client )
{
	char *ptrToEnd;
	int r;

	if (client.isRead)
		return ;
	r = recv(client.getSocket(), client.request + client.bytesRead, MIN_TO_READ, 0);
	if (r <= 0)
	{
		client.isConnected = false;
		return ;
	}
	client.bytesRead += r;
	client.request[client.bytesRead] = '\0';
	ptrToEnd = strstr(client.request, "\r\n\r\n");
	if (client.bytesRead == MAX_RQ && !ptrToEnd)
	{
		client.clientResponse.setResponse(client.formError(413, "HTTP/1.1 413 Entity Too Large\r\n", "Entity Too Large"));
		client.clientResponse.setBool(true);
		client.typeCheck = POLLOUT;
		return ;
	}
	if (ptrToEnd)
	{
		client.stringRequest = client.request;
		client.bytesRead -= (ptrToEnd - client.request + 4);
		memmove(client.request, ptrToEnd + 4, client.bytesRead + 1);
		client.isRead = true;
	}
}

void Webserver::_parseRequestLine( Client &client )
{
	int i1, i2;
	
	if (!client.isRead || client.clientResponse.getBool() || client.isRqLineParsed)
		return ;
	
	// Here parsing the request line into 3 parts
	i1 = client.stringRequest.find(' ');
	client.parsedRequest.setMethod(client.stringRequest.substr(0, i1));
	i2 = client.stringRequest.find(' ', i1 + 1);
	client.parsedRequest.setUri(client.stringRequest.substr(i1 + 1, i2 - i1 - 1));
	i1 = client.stringRequest.find('\r', i2 + 1);
	client.parsedRequest.setVersion(client.stringRequest.substr(i2 + 1, i1 - i2 - 1));
	client.isRqLineParsed = true;

	// Here I erase the request line to have just the headers
	client.stringRequest.erase(0, client.stringRequest.find("\r\n") + 2);
	// This function checks if the request line is well formed or not
	client.checkRequestLine();
}

void Webserver::_parseHeaders( Client &client )
{
	std::string first, second;
	int index;
	bool isHeader;

	// Pour l'optimisation je peux mémoriser la position du premier CRLF
	if (!client.isRqLineParsed || client.clientResponse.getBool() || client.isHeaderParsed)
		return ;
	isHeader = false;
	while (1)
	{
		index = client.stringRequest.find(':');
		first = client.stringRequest.substr(0, index);
		second = client.stringRequest.substr(index + 2, client.stringRequest.find('\r') - index - 2);
		// client.checkBody(first, second);
		client.parsedRequest.insertHeader(std::make_pair(first, second));
		if (isHeader)
		{
			client.stringRequest.erase(0, client.stringRequest.find('\r') + 4);
			break;
		}
		client.stringRequest.erase(0, client.stringRequest.find('\n') + 1);
		index = client.stringRequest.find('\r');
		if (client.stringRequest[index + 2] == '\r')
			isHeader = true;
	}
	client.checkHeaders();
	client.isHeaderParsed = true;
	// Here I should check the type of reading (chunked, normal, multipart)
	if (client.clientResponse.getBool() || !client.shouldReadBody)
		return ;
	parser.chooseCorrectParsingMode(client);
}

// Temporary function
void Webserver::_prepareResponse( Client &client )
{
	// This condition checks if the response is ready or not
	if (client.clientResponse.getBool() || !client.isHeaderParsed
		|| (client.shouldReadBody && !client.finishedBody))
		return ;
	client.typeCheck = POLLOUT;
	if (client.parsedRequest._method == "GET")
		this->_prepareGetResponse(client);
	else if (client.parsedRequest._method == "POST" && client.finishedBody)
	{
		client.clientResponse.setBool(true);
		client.clientResponse.setResponse("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 14\r\n\r\n<h1>HELLO</h1>");
		// this->_preparePostResponse(client);
	}
	else if (client.parsedRequest._method == "DELETE")
	{
		std::cout << "Delete method should be handled here" << std::endl;
		// this->_prepareDeleteResponse(client);
	}
}

void Webserver::_readBodyIfPossible( Client &client )
{
	int r;
	char buff[MIN_TO_READ + 1];

	if (!client.shouldReadBody || client.finishedBody)
		return ;
	// Should protect recv here
	r = recv(client.getSocket(), buff, MIN_TO_READ, 0);
	if (r <= 0)
	{
		client.isConnected = false;
		return ;
	}
	buff[r] = '\0';
	// Should change this
	int b = client.bytesRead;
	for (int i = 0; i < r; i++)
		client.request[b++] = buff[i];
	client.request[b] = '\0';
	client.bytesRead += r;
	parser.chooseCorrectParsingMode(client);
}

void Webserver::_prepareGetResponse( Client &client )
{
	// This is temporary
	std::string response;
	std::ifstream fileToSend;
	std::stringstream s, s2;

	fileToSend.open("upload.html");
	s << fileToSend.rdbuf();
	s2 << s.str().size();
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
	response +=  s2.str() + "\r\n\r\n" + s.str();
	// Here's an example of the end of this code
	client.clientResponse.setResponse(response);
	client.clientResponse.setBool(true);
}

void Webserver::_readAndParse( Client &client )
{
	this->_readBodyIfPossible(client);
	this->_readRequest(client);
	this->_parseRequestLine(client);
	this->_parseHeaders(client);
	this->_prepareResponse(client);
}

void Webserver::_dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend )
{
	if (shouldSend)
		it->clientResponse.sendResponse(it->getSocket());
	close(it->getSocket());
	it = _pendingClients.erase(it);
	*inc = false;
}