#include "Webserver.class.hpp"

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients(), _listeningSockets(),\
	_fdToCheck(NULL) { }

Webserver::Webserver( std::list < Serverblock > &rhs ) : _serverBlocks(rhs), _pendingClients(),\
	_listeningSockets(), _fdToCheck(NULL) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
	_pendingClients(rhs._pendingClients), _listeningSockets(rhs._listeningSockets), \
	_fdToCheck(rhs._fdToCheck) { }

Webserver::~Webserver( void ) { }

void Webserver::setServerBlocks( std::list < Serverblock > list )
{
	this->_serverBlocks = list;
}

void Webserver::createSockets( void )
{
	std::list< Serverblock >::iterator b;
	int sock, tmp;

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
		b->socketNeeds.sin_port = htons(b->get_port());
		b->socketNeeds.sin_family = PF_INET;
		b->socketNeeds.sin_addr.s_addr = 0;
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
	std::list< Serverblock >::iterator blIter;
	std::list< int >::iterator sockIter;
	socklen_t sizeOfSockaddr_in;
	int newFd, i;

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
			newClient.correspondingBlock = new Serverblock(*blIter);
			newClient.setSocket(newFd);
			_pendingClients.push_back(newClient);
		}
		i++;
		blIter++;
	}
}

void Webserver::_readAndParse( Client &client )
{
	this->_readBodyIfPossible(client);
	this->_readRequest(client);
	this->_parseRequestLine(client);
	this->_parseHeaders(client);
	this->_prepareResponse(client);
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
		return Utils::setErrorResponse(413, "HTTP/1.1 413 Entity Too Large\r\n", "Entity Too Large", client);
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
	
	i1 = client.stringRequest.find(' ');
	client.parsedRequest.setMethod(client.stringRequest.substr(0, i1));
	i2 = client.stringRequest.find(' ', i1 + 1);
	client.parsedRequest.setUri(client.stringRequest.substr(i1 + 1, i2 - i1 - 1));
	i1 = client.stringRequest.find('\r', i2 + 1);

	client.parsedRequest.setVersion(client.stringRequest.substr(i2 + 1, i1 - i2 - 1));
	client.isRqLineParsed = true;
	client.stringRequest.erase(0, client.stringRequest.find("\r\n") + 2);
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
	if (client.clientResponse.getBool() || !client.shouldReadBody)
		return ;
	parser.chooseCorrectParsingMode(client);
}

void Webserver::_readBodyIfPossible( Client &client )
{
	int r, b;
	char buff[MIN_TO_READ + 1];

	if (!client.shouldReadBody || client.finishedBody)
		return ;
	r = recv(client.getSocket(), buff, MIN_TO_READ, 0);
	if (r <= 0)
	{
		client.isConnected = false;
		return ;
	}
	buff[r] = '\0';
	b = client.bytesRead;
	for (int i = 0; i < r; i++)
		client.request[b++] = buff[i];
	client.request[b] = '\0';
	client.bytesRead += r;
	parser.chooseCorrectParsingMode(client);
}

void Webserver::_dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend )
{
	if (shouldSend)
		it->clientResponse.sendResponse(it->getSocket());
	close(it->getSocket());
	it = _pendingClients.erase(it);
	*inc = false;
}

void Webserver::_prepareResponse( Client &client )
{
	std::list< Location >::iterator currentList;

	if (client.clientResponse.getBool() || !client.isHeaderParsed
		|| (client.shouldReadBody && !client.finishedBody))
		return ;
	client.typeCheck = POLLOUT;
	client.currentList = client.correspondingBlock->ifUriMatchLocationBlock(client.correspondingBlock->_location, client.parsedRequest._uri);
	if (client.currentList == client.correspondingBlock->_location.end())
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "404 File Not Found", client);
	else if (client.currentList->get_isThereRedirection())
		return _handleHttpRedirection(client.currentList, client);
	else if (!client.currentList->isMethodAccepted(client.currentList, client.parsedRequest._method))
		return Utils::setErrorResponse(405, "HTTP/1.1 405 Not Allowed", "405 Method Not Allowed", client);
	else if (!client.currentList->checkIfPathExist(client.currentList->_currentRoot))
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "404 Not Found", client);
	else if (client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot)
		&& !client.currentList->checkIfPathIsValid(client.currentList->_currentRoot, client.parsedRequest._uri, client.clientResponse, client.currentList->get_root_location()))
		return ;
	this->_handleProperResponse(client);
}

void Webserver::_handleProperResponse( Client &client )
{
	// Here beggins the methods implementations
	if (client.parsedRequest._method == "GET")
		this->_prepareGetResponse(client);
	else if (client.parsedRequest._method == "POST")
		this->_preparePostResponse(client);
	else if (client.parsedRequest._method == "DELETE")
		this->_prepareDeleteResponse(client);
}

void Webserver::_handelFolderRequest(Client &client)
{
	DIR *dir;
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;
	std::ifstream file;

	for(; index != client.currentList->_indexes_location.end(); ++index)
	{
		joinPath = client.currentList->_currentRoot + (*index);
		file.open(joinPath, std::ios::binary);
		if(file.good())
		{
			if(client.currentList->get_cgi())
				_runCgi(joinPath, client);
			else
			{
				std::string response =  "HTTP/1.1 200 Ok\r\nContent-Length : " + Utils::getSizeOfFile(file) +  " 	Content-Type : " + client.parsedRequest._headers["Content-Type"] + "\r\n\r\n";
				response += Utils::getFileContent(file);
				Utils::setGoodResponse(response, client);
			}
			return ;
		}
	}
	if(client.currentList->get_autoindex())
	{
		if((dir = opendir(client.currentList->_currentRoot.c_str())))
		{
			std::string response = "HTTP/1.1 200 Ok\r\nContent-Length : " + Utils::getSizeOfFile(file) +  client.parsedRequest._headers["Content-Type"] + "\r\n\r\n";
			response += Utils::handleAutoindexFolder(client.currentList->_currentRoot.c_str()); 
			Utils::setGoodResponse(response, client);
		}
	}
	else
	{
		if(client.currentList->_indexes_location.empty())
			Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error", "403 Forbidden error", client);
		else
			Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "404 File Not Found", client);
	}

}

void	Webserver::_handelFileRequest(Client &client)
{
	std::ifstream file;

	file.open(client.currentList->_currentRoot, std::ios::binary);
	if(file.is_open())
	{
		std::string response = "HTTP/1.1 200 Ok\r\nContent-Length :" + Utils::getSizeOfFile(file) + "\r\n\r\n";
		response += Utils::getFileContent(file);
		Utils::setGoodResponse(response, client);
	}
	else
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "404 File Not Found", client);
}

void	Webserver::_readFile(std::string &path, Client &client, std::string &name)
{
	std::ifstream file(path.c_str());
	std::stringstream buffer;
	std::string 	  str;
	std::string       body;
	std::string 	  response;
	size_t 			  find;

	find = name.find_last_of(".");
	buffer << file.rdbuf();
	str = buffer.str();
	buffer.str("");
	size_t findBody = str.find("\r\n\r\n");
	body = str;
	if(findBody != std::string::npos)
		body = str.substr(findBody + 4, str.length() - (findBody + 4));
	buffer << body.length();
	response = "HTTP/1.1 200 OK\r\nContent-Length: " + buffer.str() + "\r\n";
	if(find != std::string::npos && name.substr(find + 1, name.length()) == "py")
		response += "Content-Type: text/html\r\n\r\n";
	response += str;
	Utils::setGoodResponse(response, client);
}

void Webserver::_runCgi(std::string &name, Client &client)
{
	int fd;
	char *args[3];
	char *env[] = {(char *)("PATH_INFO=" + Utils::getPathInfo() + name).c_str(), NULL};
	std::string path = "/tmp/temp";
	
	fd = open(path.c_str(), O_CREAT | O_RDWR, 0664);
	size_t findFileExtension = name.find_last_of(".");
	if(findFileExtension != std::string::npos)
	{
		args[0] = strdup((Utils::getPathInfo() + "/" + "php-cgi").c_str());
		if(name.substr(findFileExtension + 1, name.length()) == "py")
		{
			free(args[0]);
			args[0] = strdup("/usr/bin/python");
		}
		if (!client.nameForCgi.empty() && client.isThereCgi	)
			name = client.nameForCgi;
		args[1] = strdup((name).c_str());
		args[2] = NULL;
	}
	if(fork() == 0)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
		if(execve(args[0], args, NULL) < 0)
			Utils::setErrorResponse(404, "HTTP/1.1 500 Internal Server Error", "500 Internal Server Error", client);
	}

	while(wait(NULL) != -1);
	_readFile(path, client, name);
	close(fd);
	if (!client.nameForCgi.empty() && client.isThereCgi	)
		unlink(name.c_str());
	unlink(path.c_str());
}

void Webserver::_prepareGetResponse( Client &client )
{
	if(client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot))
		_handelFolderRequest(client);
	else
	{
		if(client.currentList->get_cgi())
			_runCgi(client.currentList->_currentRoot, client);
		else
			_handelFileRequest(client);
	}
}

void Webserver::_preparePostResponse( Client &client )
{
	// _handleCgi(client.currentList, client, client.currentList->_currentRoot);
	if (client.isThereCgi)
		_runCgi(client.currentList->_currentRoot, client);
	else
		Utils::setGoodResponse("HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: 36\r\n\r\n<h1>File uploaded succesfully !</h1>", client);
}

void Webserver::_prepareDeleteResponse( Client &client )
{
	Utils::setGoodResponse("HTTP/1.1 OK 200\r\nContent-Type: text/html\r\nContent-Length: 28\r\n\r\n<h1>Hello from DELETE !</h1>", client);
}

void Webserver::_handleHttpRedirection(std::list<Location>::iterator &currentList, Client &client)
{
	Utils::setGoodResponse("HTTP/1.1 301 Moved Permanently\r\nLocation: " + currentList->_redirection[1] + "\r\n" + "Content-Length :0\r\n", client);
}

