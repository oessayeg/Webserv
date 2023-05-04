#include "../includes/Webserver.class.hpp"

// In each function that parses the request, a checking function will be called.
// In all checking functions, if something is wrong, they will 
// automatically prepare an error response and stop the process of parsing.

Webserver::Webserver( void ) : _serverBlocks(), _pendingClients(), _listeningSockets(),\
	_fdToCheck(NULL) { }

Webserver::Webserver( std::list < Serverblock > &rhs ) : _serverBlocks(rhs), _pendingClients(),\
	_listeningSockets(), _fdToCheck(NULL) { }

Webserver::Webserver( const Webserver &rhs ) : _serverBlocks(rhs._serverBlocks), \
	_pendingClients(rhs._pendingClients), _listeningSockets(rhs._listeningSockets), \
	_fdToCheck(rhs._fdToCheck) { }

Webserver &Webserver::operator=( const Webserver &rhs )
{
	if (this != &rhs)
	{
		this->_serverBlocks = rhs._serverBlocks;
		this->_pendingClients = rhs._pendingClients;
		this->_listeningSockets = rhs._listeningSockets;
		*this->_fdToCheck = *rhs._fdToCheck;
		this->_parser = rhs._parser;
	}
	return (*this);
}

Webserver::~Webserver( void )
{
	if (_fdToCheck)
		delete _fdToCheck;
}

// This function gets the list of the blocks after parsing the config file.
// This list will be pointed to by clients to serve them depending on it.
void Webserver::setServerBlocks( std::list < Serverblock > &list )
{
	this->_serverBlocks = list;
}

// This function creates the sockets with the appropriate ip and port.
// These sockets will be able to listen to incoming connections.
void Webserver::createSockets( void )
{
	std::list< Serverblock >::iterator b;
	int sock, tmp;

	sock = 0;
	tmp = 1;
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

// This function initializes the pollfd struct with all available sockets
// (clients/listening sockets) for future read/write operations on them.
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

// This is the main function that'll parse the request and prepare the appropriate response.
void Webserver::readAndRespond( void )
{
	std::list< Client >::iterator b;
	int sizeOfSocketsAndClients;
	int nbFds;
	int i;
	bool increment;

	increment = true;
	sizeOfSocketsAndClients = this->_listeningSockets.size() + this->_pendingClients.size();
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

// If a listening socket is ready for reading, this function will be called,
// it will accept connections and add a new client to be served into the client list.
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

// These are the steps to parse and prepare the response.
// Each function here depends on the one called before it
// except for the first one, it depends on the existence of a body.
void Webserver::_readAndParse( Client &client )
{
	this->_readBodyIfPossible(client);
	this->_readRequest(client);
	this->_parseRequestLine(client);
	this->_parseHeaders(client);
	this->_prepareResponse(client);
}

// After accepting a connection and finding out that it is ready for reading
// recv() is used, it'll have the request sent by the client.
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
	if (client.bytesRead == MIN_TO_READ && !ptrToEnd)
		return Utils::setErrorResponse(413, "HTTP/1.1 413 Entity Too Large\r\n", "Entity Too Large", client);
	if (ptrToEnd)
	{
		client.stringRequest = client.request;
		client.bytesRead -= (ptrToEnd - client.request + 4);
		memmove(client.request, ptrToEnd + 4, client.bytesRead + 1);
		client.isRead = true;
	}
}

// After reading the whole request, parsing will succeed.
// This function parses the request line into 3 parts :
// Method, Uri, Version.
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

	i1 = client.parsedRequest.getUri().find('?');
	if (i1 != std::string::npos)
	{
		client.parsedRequest.setQueryString(client.parsedRequest.getUri().substr(i1 + 1));
		client.parsedRequest.setUri(client.parsedRequest.getUri().substr(0, i1));
	}
	Utils::checkRequestLine(client);
}

// After finishing the request line parsing, headers will be put in a map.
// If the request is POST, the process of checking if a list matches/there is an upload directory...
// will be started and the body reading also will start.
void Webserver::_parseHeaders( Client &client )
{
	std::string first, second;
	int index;
	bool isHeader;

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
	Utils::checkHeaders(client);
	client.isHeaderParsed = true;
	if (client.clientResponse.getBool() || !client.shouldReadBody)
		return ;
	_parser.chooseCorrectParsingMode(client);
}

// This function will be executed only if there is a body.
// It will read the body by 8kb and at the same time parse it.
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
	_parser.chooseCorrectParsingMode(client);
}

// This function drops the client directly if the connection was closed or
// the recv function failed, or send the response if it is ready and drops the client.
void Webserver::_dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend )
{
	if (shouldSend && it->clientResponse.readFromFile() && !_sendFile(it))
			return ;
	else if (shouldSend)
		it->clientResponse.sendResponse(it->getSocket());
	close(it->getSocket());
	it = _pendingClients.erase(it);
	*inc = false;
}

// After parsing the request and headers the response should be prepared and sent.
// This function checks if the uri matches a location block and the validity of that block with the request.
// If everything went right, a response will be set and sent.
void Webserver::_prepareResponse( Client &client )
{
	std::list< Location >::iterator currentList;

	if (client.clientResponse.getBool() || !client.isHeaderParsed
		|| (client.shouldReadBody && !client.finishedBody))
		return ;
	if (client.correspondingBlock->_serverNames.size() > 0 && !Utils::serverNameMatches(client.parsedRequest.getValueFromMap("Host"), client.correspondingBlock))
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", client);
	client.currentList = client.correspondingBlock->ifUriMatchLocationBlock(client.correspondingBlock->_location, client.parsedRequest.getUri());
	if (client.currentList == client.correspondingBlock->_location.end())
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", client);
	else if (client.currentList->get_isThereRedirection())
		return _handleHttpRedirection(client.currentList, client);
	else if (!client.currentList->isMethodAccepted(client.currentList, client.parsedRequest.getMethod()))
		return Utils::setErrorResponse(405, "HTTP/1.1 405 Not Allowed\r\n", "Method Not Allowed", client);
	else if (!client.currentList->checkIfPathExist(client.currentList->_currentRoot))
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "Not Found", client);
	else if (client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot)
		&& (!client.currentList->checkIfPathIsValid(client.currentList->_currentRoot, client.clientResponse) && client.parsedRequest.getMethod() != "DELETE"))
		return ;
	this->_handleProperResponse(client);
}

// This function prepares a response depending on the method.
void Webserver::_handleProperResponse( Client &client )
{
	if (client.parsedRequest.getMethod() == "GET")
		this->_prepareGetResponse(client);
	else if (client.parsedRequest.getMethod() == "POST")
		this->_preparePostResponse(client);
	else if (client.parsedRequest.getMethod() == "DELETE")
		this->_prepareDeleteResponse(client);
}

// Get method handler.
// This function will get the files requested and set them in the body of the response.
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

// Post method handler (if a cgi should be executed).
// If not then the file will already be uploaded.
void Webserver::_preparePostResponse( Client &client )
{
	std::string name = Utils::getIndex(client);
	if (client.isThereCgi)
		_runCgi(name, client);
	else
		Utils::setGoodResponse("HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: 36\r\n" + Utils::getDateAndTime() + "<h1>File uploaded succesfully !</h1>", client);
}

// Delete method handler.
// This function will check for permissions, existence of folder/files then delete them.
void Webserver::_prepareDeleteResponse( Client &client )
{
	if(client.currentList->ifRequestUriIsFolder(client.currentList->_currentRoot))
		_handleDeleteFolderRequest(client);
	else
		_handleDeleteFile(client);
}

// Redirect the client, if there is a redirection in the config file.
void Webserver::_handleHttpRedirection(std::list<Location>::iterator &currentList, Client &client)
{
	Utils::setGoodResponse("HTTP/1.1 301 Moved Permanently\r\nLocation: " + currentList->_redirection[1] + "\r\n" + "Content-Length: 0\r\n" + Utils::getDateAndTime(), client);
}

// This function prepares args, env variables for the cgi and executes it.
// It will be executed on a file for GET/DELETE requests, and on the bodies for POST requests.
// The files created and used here are all removed.
void Webserver::_runCgi(std::string &name, Client &client)
{
	char **args;
	char **env;
	int status;
	int fd;

	env = _prepareCgiEnv(client, name);
	fd = open("/tmp/temp", O_CREAT | O_RDWR | O_TRUNC, 0664);
	args = _prepareArgs(name);
	if(fork() == 0)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
		if (client.parsedRequest.getMethod() == "POST")
		{
			fd = open(client.nameForCgi.c_str(), O_RDONLY);
			dup2(fd, 0);
			close(fd);
		}
		if(execve(args[0], args, env) < 0)
			exit(EXIT_FAILURE);
	}

	while (waitpid(-1, &status, WNOHANG) != -1) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            break;
        }
        usleep(10000);
    }

    _readFile("/tmp/temp", client, name);
    close(fd);
    unlink("/tmp/temp");
    if (client.parsedRequest.getMethod() == "POST") {
        unlink(client.nameForCgi.c_str());
    }
	
}

// This function is called in runCgi, it will get the content of a file and put
// it as a response.
// This file contains the output of the cgi.
void	Webserver::_readFile(std::string path, Client &client, std::string &name)
{
	std::ifstream file(path.c_str());
	std::stringstream buffer;
	std::string 	  str;
	std::string       body;
	std::string 	  response;
	std::string		  date;
	size_t 			  find;

	date = Utils::getDateAndTime();
	date.pop_back();
	date.pop_back();
	find = name.find_last_of(".");
	buffer << file.rdbuf();
	str = buffer.str();
	buffer.str("");
	size_t findBody = str.find("\r\n\r\n");
	body = str;
	if(findBody != std::string::npos)
		body = str.substr(findBody + 4, str.length() - (findBody + 4));
	buffer << body.length();
	response = "HTTP/1.1 200 OK\r\n" + date + "Content-Length: " + buffer.str() + "\r\n";
	if(find != std::string::npos && name.substr(find + 1, name.length()) == "py")
		response += "Content-Type: text/html\r\n\r\n";
	response += str;
	Utils::setGoodResponse(response, client);
}

// Main function that handles delete.
// It recursively goes to subdirectories of the request directory and deletes their content.
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
					return Utils::setErrorResponse(500, "HTTP/1.1 500 Internal Server Error\r\n", "Internal Server Error", client);
				}
                status = remove(fullPath.c_str());
				if(status != 0)
				{
					shouldPrint = false;
					closedir(dir);
					return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error\r\n", "Forbidden error", client);
				}
            }
		}
		closedir(dir);
	}
}

// This function is called in GET request, it will handle folder requests.
void Webserver::_handleFolderRequest(Client &client)
{
	DIR *dir;
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;
	std::string indexes;

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
				client.clientResponse.setReadFromFile(true);
				client.clientResponse.setBool(true);
				client.clientResponse.status = "HTTP/1.1 200 Ok\r\nContent-Type: " + _parser.getContentType(joinPath);
				client.clientResponse.status += "\r\nContent-Length: " + Utils::getSizeOfFile(joinPath) + "\r\n" + Utils::getDateAndTime();
				client.clientResponse.setFileSize(Utils::getSize(joinPath));
			}
			return ;
		}
	}
	if(client.currentList->get_autoindex())
	{
		if((dir = opendir(client.currentList->_currentRoot.c_str())))
		{
			indexes = Utils::handleAutoindexFolder(client.currentList->_currentRoot.c_str());
			std::string response = "HTTP/1.1 200 Ok\r\nContent-Length: " + Utils::getSizeInString(indexes) + "\r\nContent-Type: text/html\r\n" + Utils::getDateAndTime();
			response += indexes;
			Utils::setGoodResponse(response, client);
		}
		closedir(dir);
	}
	else
	{
		if(client.currentList->_indexes_location.empty())
			return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error\r\n", "Forbidden error", client);
		return Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", client);
	}
}

// This function gets the requested file and puts it in the response.
void	Webserver::_handleFileRequest(Client &client)
{
	client.clientResponse.file.open(client.currentList->_currentRoot, std::ios::binary); 
	if(client.clientResponse.file.is_open())
	{
		client.clientResponse.status = "HTTP/1.1 200 Ok\r\nContent-Length: " + Utils::getSizeOfFile(client.currentList->_currentRoot);
		client.clientResponse.status += "\r\nContent-Type: " + _parser.getContentType(client.currentList->_currentRoot) + "\r\n" + Utils::getDateAndTime();
		client.clientResponse.setReadFromFile(true);
		client.clientResponse.setBool(true);
		client.clientResponse.setFileSize(Utils::getSize(client.currentList->_currentRoot));
	}
	else
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", client);
}

// This function handles delete folder requests.
void Webserver::_handleDeleteFolderRequest(Client &client)
{
	int status = -1;
	bool	shouldPrint = true;
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;
	std::ifstream file;

	if(client.currentList->_currentRoot[client.currentList->_currentRoot.length() - 1] != '/')
	{
		Utils::setErrorResponse(409, "HTTP/1.1 409 Conflict\r\n", "Conflict", client);
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
		Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden\r\n", "Forbidden", client);
	}
	else
	{
		_removeContent(client.currentList->_currentRoot, client , status, shouldPrint);
		if(status == 0)
			return Utils::setGoodResponse("HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: 35\r\n" + Utils::getDateAndTime() + "<h1> File Deleted successfully</h1>", client);
		else if(status == -1 && shouldPrint)
			return Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden error\r\n", "Forbidden", client);
	}
}

// This one handles file DELETE requests.
// It checks if the file requested exists/has permissions and deletes it.
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
			return Utils::setGoodResponse("HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: 36\r\n" + Utils::getDateAndTime() + "<h1> File Deleted successfully </h1>", client);
		}
		file.close();
	}
	Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden\r\n", "Forbidden", client);
}

// If the response contains a body, it will be sent 1536 by 1536 bytes.
// This function is responsible for that.
bool Webserver::_sendFile( std::list< Client >::iterator &it )
{
	char buff[1536 + 1];
	std::streamsize bytes;

	it->clientResponse.file.read(buff, 1536);
	bytes = it->clientResponse.file.gcount();
	it->clientResponse.incrementBytesFromFile(bytes);

	if (!it->clientResponse.getIsStatusSent())	
		return _sendWithStatusCode(it, bytes, buff);
	if (send(it->getSocket(), buff, bytes, 0) <= 0 || it->clientResponse.getFileSize() == it->clientResponse.getBytesFromFile())
	{
		it->clientResponse.file.close();
		return true;
	}
	return false;
}

// For the first chunk of data sent, the request line and headers are sent here.
bool Webserver::_sendWithStatusCode( std::list< Client >::iterator &it, int bytes, char *buff )
{
	char buff2[1536 + 1 + it->clientResponse.status.size()];
	size_t i, x;

	it->clientResponse.setIsStatusSent(true);
	for (i = 0; i < it->clientResponse.status.size(); i++)
		buff2[i] = it->clientResponse.status[i];
	x = 0;
	for (; i < bytes + it->clientResponse.status.size(); i++)
		buff2[i] = buff[x++];
	if (send(it->getSocket(), buff2, i, 0) <= 0 || it->clientResponse.getFileSize() == it->clientResponse.getBytesFromFile())
	{
		it->clientResponse.file.close();
		return true;
	}
	return false;
}

// This function sets the appropriate environment variables for the cgi.
char **Webserver::_prepareCgiEnv( Client &client, std::string &name )
{
	char **retEnv;

	retEnv = new char*[12];
	retEnv[0] = Utils::giveAllocatedChar("PATH_INFO=" + Utils::getPathInfo() + "/" + name);
	retEnv[1] = Utils::giveAllocatedChar("GATEWAY_INTERFACE=CGI/1.1");
	retEnv[2] = Utils::giveAllocatedChar("REQUEST_METHOD=" + client.parsedRequest.getMethod());
	retEnv[3] = Utils::giveAllocatedChar("SCRIPT_NAME=" + Utils::getPathInfo() + "/" + name);
	retEnv[4] = Utils::giveAllocatedChar("SCRIPT_FILENAME=" + Utils::getPathInfo() + "/" + name);
	retEnv[5] = Utils::giveAllocatedChar("REDIRECT_STATUS=200");
	retEnv[6] = Utils::giveAllocatedChar("SERVER_PROTOCOL=HTTP/1.1");
	retEnv[7] = Utils::giveAllocatedChar("QUERY_STRING=" + client.parsedRequest.getQueryString());
	retEnv[8] = Utils::giveAllocatedChar("HTTP_COOKIE=" + client.parsedRequest.getValueFromMap("Cookie"));
	if (client.parsedRequest.getMethod() == "POST")
	{
		retEnv[9] = Utils::giveAllocatedChar("CONTENT_TYPE=" + client.parsedRequest.getValueFromMap("Content-Type"));
		retEnv[10] = Utils::giveAllocatedChar("CONTENT_LENGTH=" + client.parsedRequest.getValueFromMap("Content-Length"));
		retEnv[11] = NULL;
	}
	else
		retEnv[9] = NULL;
	return retEnv;
}

// This function sets the arguments that will be sent to execve.
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