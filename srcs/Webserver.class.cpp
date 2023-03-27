#include "../includes/Webserver.class.hpp"

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients(), _listeningSockets(),\
	_fdToCheck(NULL) { }

Webserver::Webserver( std::list < Serverblock > &rhs ) : _serverBlocks(rhs), _pendingClients(),\
	_listeningSockets(), _fdToCheck(NULL) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
	_pendingClients(rhs._pendingClients), _listeningSockets(rhs._listeningSockets), \
	_fdToCheck(rhs._fdToCheck) { }

Webserver::~Webserver( void )
{
	if (_fdToCheck)
		delete _fdToCheck;
}

void Webserver::setServerBlocks( std::list < Serverblock > &list )
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
		b->socketNeeds.sin_addr.s_addr = b->get_ip();
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
		_fdToCheck[i].events = POLLIN | POLLOUT;
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
		if (!b->isConnected || _fdToCheck[i].revents & POLLHUP)
			_dropClient(b, &increment, 0);
		else if (b->clientResponse.getBool() && _fdToCheck[i].revents & POLLOUT)
			_dropClient(b, &increment, 1);
		if (increment)
			b++;
		increment = true;
	}
	delete _fdToCheck;
	_fdToCheck = NULL;
}

void Webserver::_acceptNewClients( void )
{
	std::list< Serverblock >::iterator blIter;
	std::list< int >::iterator sockIter;
	socklen_t sizeOfSockaddr_in;
	int newFd, i, tmp = 1;

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
			setsockopt(newFd, SOL_SOCKET, SO_NOSIGPIPE, &tmp, sizeof(tmp));
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
	size_t i1, i2;
	
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

	i1 = client.parsedRequest._uri.find('?');
	if (i1 != std::string::npos)
	{
		client.parsedRequest._queryStr = client.parsedRequest._uri.substr(i1 + 1);
		client.parsedRequest._uri = client.parsedRequest._uri.substr(0, i1);
	}
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

bool Webserver::_sendWithStatusCode( std::list< Client >::iterator &it, int bytes, char *buff )
{
	char buff2[1536 + 1 + it->clientResponse._status.size()];
	size_t i, x;

	it->clientResponse._isStatusSent = true;
	for (i = 0; i < it->clientResponse._status.size(); i++)
		buff2[i] = it->clientResponse._status[i];
	x = 0;
	for (; i < bytes + it->clientResponse._status.size(); i++)
		buff2[i] = buff[x++];
	if (send(it->getSocket(), buff2, i, 0) <= 0 || it->clientResponse._fileSize == it->clientResponse.r)
	{
		it->clientResponse.file.close();
		return true;
	}
	return false;
}

bool Webserver::_sendFile( std::list< Client >::iterator &it )
{
	char buff[1536 + 1];
	std::streamsize bytes;

	it->clientResponse.file.read(buff, 1536);
	bytes = it->clientResponse.file.gcount();
	it->clientResponse.r += bytes;

	if (!it->clientResponse._isStatusSent)	
		return _sendWithStatusCode(it, bytes, buff);
	if (send(it->getSocket(), buff, bytes, 0) <= 0 || it->clientResponse._fileSize == it->clientResponse.r)
	{
		it->clientResponse.file.close();
		return true;
	}
	return false;
}

void Webserver::_dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend )
{
	if (shouldSend && it->clientResponse._shouldReadFromFile && !_sendFile(it))
			return ;
	else if (shouldSend)
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
	if (!Utils::serverNameMatches(client.parsedRequest._headers["Host"], client.correspondingBlock))
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "File Not Found", client);
	client.currentList = client.correspondingBlock->ifUriMatchLocationBlock(client.correspondingBlock->_location, client.parsedRequest._uri);
	if (client.currentList == client.correspondingBlock->_location.end())
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "File Not Found", client);
	else if (client.currentList->get_isThereRedirection())
		return _handleHttpRedirection(client.currentList, client);
	else if (!client.currentList->isMethodAccepted(client.currentList, client.parsedRequest._method))
		return Utils::setErrorResponse(405, "HTTP/1.1 405 Not Allowed", "Method Not Allowed", client);
	else if (!client.currentList->checkIfPathExist(client.currentList->_currentRoot))
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "Not Found", client);
	else if (client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot)
		&& (!client.currentList->checkIfPathIsValid(client.currentList->_currentRoot, client.clientResponse) && client.parsedRequest._method != "DELETE"))
		return ;
	this->_handleProperResponse(client);
}

void Webserver::_handleProperResponse( Client &client )
{
	if (client.parsedRequest._method == "GET")
		this->_prepareGetResponse(client);
	else if (client.parsedRequest._method == "POST")
		this->_preparePostResponse(client);
	else if (client.parsedRequest._method == "DELETE")
		this->_prepareDeleteResponse(client);
}

void Webserver::_handleFolderRequest(Client &client)
{
	DIR *dir;
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;

	for(; index != client.currentList->_indexes_location.end(); ++index)
	{
		joinPath = client.currentList->_currentRoot + (*index);
		client.clientResponse.file.open(joinPath, std::ios::binary);
		if(client.clientResponse.file.is_open())
		{
			if(client.currentList->get_cgi())
				_runCgi(joinPath, client);
			else
			{
				client.clientResponse._shouldReadFromFile = true;
				client.clientResponse._nameOfFile =  joinPath;
				client.typeCheck = POLLOUT;
				client.clientResponse.setBool(true);
				client.clientResponse._status = "HTTP/1.1 200 Ok\r\nContent-Type: " + parser.getContentType(joinPath);
				client.clientResponse._status += "\r\nContent-Length: " + Utils::getSizeOfFile(joinPath) + "\r\n\r\n";
				client.clientResponse._fileSize = Utils::getSize(joinPath);
			}
			return ;
		}
	}
	if(client.currentList->get_autoindex())
	{
		if((dir = opendir(client.currentList->_currentRoot.c_str())))
		{
			std::string response = "HTTP/1.1 200 Ok\r\nContent-Length: " + Utils::getSizeOfFile(client.currentList->_currentRoot) + "\r\nContent-Type: text/html\r\n\r\n";
			response += Utils::handleAutoindexFolder(client.currentList->_currentRoot.c_str());
			Utils::setGoodResponse(response, client);
		}
		closedir(dir);
	}
	else
	{
		if(client.currentList->_indexes_location.empty())
			return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error", "Forbidden error", client);
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "File Not Found", client);
	}
}

void	Webserver::_handleFileRequest(Client &client)
{
	client.clientResponse.file.open(client.currentList->_currentRoot, std::ios::binary); 
	if(client.clientResponse.file.is_open())
	{
		client.clientResponse._status = "HTTP/1.1 200 Ok\r\nContent-Length: " + Utils::getSizeOfFile(client.currentList->_currentRoot);
		client.clientResponse._status += "\r\nContent-Type: " + parser.getContentType(client.currentList->_currentRoot) + "\r\n\r\n";
		client.clientResponse._shouldReadFromFile = true;
		client.clientResponse.setBool(true);
		client.clientResponse._fileSize = Utils::getSize(client.currentList->_currentRoot);
		client.clientResponse._nameOfFile =  client.currentList->_currentRoot;
		client.typeCheck = POLLOUT;
	}
	else
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found", "File Not Found", client);
}

void	Webserver::_readFile(std::string path, Client &client, std::string &name)
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

char **Webserver::_prepareCgiEnv( Client &client, std::string &name )
{
	char **retEnv;

	retEnv = new char*[12];
	retEnv[0] = Utils::giveAllocatedChar("PATH_INFO=" + Utils::getPathInfo() + "/" + name);
	retEnv[1] = Utils::giveAllocatedChar("GATEWAY_INTERFACE=CGI/1.1");
	retEnv[2] = Utils::giveAllocatedChar("REQUEST_METHOD=" + client.parsedRequest._method);
	retEnv[3] = Utils::giveAllocatedChar("SCRIPT_NAME=" + Utils::getPathInfo() + "/" + name);
	retEnv[4] = Utils::giveAllocatedChar("SCRIPT_FILENAME=" + Utils::getPathInfo() + "/" + name);
	retEnv[5] = Utils::giveAllocatedChar("REDIRECT_STATUS=200");
	retEnv[6] = Utils::giveAllocatedChar("SERVER_PROTOCOL=HTTP/1.1");
	retEnv[7] = Utils::giveAllocatedChar("QUERY_STRING=" + client.parsedRequest._queryStr);
	retEnv[8] = Utils::giveAllocatedChar("HTTP_COOKIE=" + client.parsedRequest._headers["Cookie"]);
	if (client.parsedRequest._method == "POST")
	{
		retEnv[9] = Utils::giveAllocatedChar("CONTENT_TYPE=" + client.parsedRequest._headers["Content-Type"]);
		retEnv[10] = Utils::giveAllocatedChar("CONTENT_LENGTH=" + client.parsedRequest._headers["Content-Length"]);
		retEnv[11] = NULL;
	}
	else
		retEnv[9] = NULL;
	return retEnv;
}

char **Webserver::_prepareArgs( const std::string &name )
{
	char **args;
	size_t fileExt;

	args = new char*[3];
	args[0] = Utils::giveAllocatedChar((Utils::getPathInfo() + "/" + "php-cgi").c_str());
	args[1] = Utils::giveAllocatedChar(name);
	args[2] = NULL;
	fileExt = name.find_last_of(".");
	if(fileExt != std::string::npos && name.substr(fileExt + 1, name.length()) == "py")
	{
		delete args[0];
		args[0] = Utils::giveAllocatedChar("/usr/bin/python");
	}
	return args;
}

void Webserver::_runCgi(std::string &name, Client &client)
{
	char **args;
	char **env;
	int fd;

	env = _prepareCgiEnv(client, name);
	fd = open("/tmp/temp", O_CREAT | O_RDWR | O_TRUNC, 0664);
	args = _prepareArgs(name);
	if(fork() == 0)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
		if (client.parsedRequest._method == "POST")
		{
			fd = open(client.nameForCgi.c_str(), O_RDONLY);
			dup2(fd, 0);
			close(fd);
		}
		if(execve(args[0], args, env) < 0)
			exit(EXIT_FAILURE);
	}
	wait(NULL);
	Utils::deleteDoublePtr(args);
	Utils::deleteDoublePtr(env);
	_readFile("/tmp/temp", client, name);
	close(fd);
	unlink("/tmp/temp");
	if (client.parsedRequest._method == "POST")
		unlink(client.nameForCgi.c_str());
}

void Webserver::_prepareGetResponse( Client &client )
{
	if(client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot))
		_handleFolderRequest(client);
	else
	{
		if(client.currentList->get_cgi())
			return _runCgi(client.currentList->_currentRoot, client);
		return _handleFileRequest(client);
	}
}

std::string getIndex( Client &client )
{
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;

	for(; index != client.currentList->_indexes_location.end(); ++index)
	{
		joinPath = client.currentList->_currentRoot + (*index);
		client.clientResponse.file.open(joinPath, std::ios::binary);
		if(client.clientResponse.file.is_open())
		{
			client.clientResponse.file.close();
			return joinPath;
		}
	}
	return client.currentList->_currentRoot;
}

void Webserver::_preparePostResponse( Client &client )
{
	std::string name = getIndex(client);
	if (client.isThereCgi)
		_runCgi(name, client);
	else
		Utils::setGoodResponse("HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: 36\r\n\r\n<h1>File uploaded succesfully !</h1>", client);
}

void 			Webserver::_removeContent(const std::string &path, Client &client, int &status, bool &shouldPrint)
{
	DIR *dir;
    DIR *dir1;
	std::string fullPath;
	struct dirent *opt;

	opt = NULL;
	dir = NULL;
	dir1 = NULL;
	if((dir = opendir(path.c_str())) != NULL)
	{
		while((opt = readdir(dir)) != NULL)
        {
       		fullPath = (path + "/" + std::string(opt->d_name)).c_str();
            if((dir1= opendir(fullPath.c_str())) != NULL && strcmp(opt->d_name, ".") && strcmp(opt->d_name, ".."))
			{
                _removeContent(fullPath, client, status, shouldPrint);
				closedir(dir1);
				dir1 = NULL;
			}
			if(dir1 != NULL)
			{
				closedir(dir1);
				dir1 = NULL;
			}
			if(strcmp(opt->d_name, ".") && strcmp(opt->d_name, ".."))
            {
				if(access(fullPath.c_str(), W_OK | R_OK) == -1 )
				{
					shouldPrint = false;
					closedir(dir);
					return Utils::setErrorResponse(500, "HTTP/1.1 500 Internal Server Error", "Internal Server Error", client);
				}
                status = remove(fullPath.c_str());
				if(status != 0)
				{
					shouldPrint = false;
					closedir(dir);
					return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error", "Forbidden error", client);
				}
            }
		}
		closedir(dir);
	}
}

void Webserver::_handleDeleteFolderRequest(Client &client)
{
	int status = -1;
	bool	shouldPrint = true;
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;
	std::ifstream file;

	if(client.currentList->_currentRoot[client.currentList->_currentRoot.length() - 1] != '/')
	{
		Utils::setErrorResponse(409, "HTTP/1.1 409 Conflict", "Conflict", client);
	}
	else if(client.currentList->get_cgi())			
	{
		for(; index != client.currentList->_indexes_location.end(); ++index)
		{
			joinPath = client.currentList->_currentRoot + (*index);
			file.open(joinPath);
			if(file.is_open())
			{
				file.close();
				return _runCgi(joinPath, client);
			}
		}
		file.close();
		Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden", "Forbidden", client);
	}
	else
	{
		_removeContent(client.currentList->_currentRoot, client , status, shouldPrint);
		if(status == 0)
			return Utils::setGoodResponse("HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: 35\r\n\r\n<h1> File Deleted successfully</h1>", client);
		else if(status == -1 && shouldPrint)
			return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error", "Forbidden", client);
	}
}

void Webserver::_handleDeleteFile(Client &client)
{
	std::ifstream file;
	int status;

	file.open(client.currentList->_currentRoot, std::ios::binary);
	if(file.is_open())
	{
		if(client.currentList->get_cgi())
		{
			file.close();
			return _runCgi(client.currentList->_currentRoot, client);
		}
		status = remove(client.currentList->_currentRoot.c_str());
		if(status == 0)
		{
			file.close();
			return Utils::setGoodResponse("HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: 17\r\n\r\n<h1> DELETE </h1>", client);
		}
		file.close();
	}
	Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden", "Forbidden", client);
}

void Webserver::_prepareDeleteResponse( Client &client )
{
	if(client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot))
		_handleDeleteFolderRequest(client);
	else
		_handleDeleteFile(client);
}

void Webserver::_handleHttpRedirection(std::list<Location>::iterator &currentList, Client &client)
{
	Utils::setGoodResponse("HTTP/1.1 301 Moved Permanently\r\nLocation: " + currentList->_redirection[1] + "\r\n" + "Content-Length: 0\r\n\r\n", client);
}