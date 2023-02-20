#include "Webserver.hpp"

// Network byte order is big-endian, so htons make sure that our nbr is in big-endian form\
// if our machine stores nbrs in little endian format

Webserver::Webserver( void ) : _socket(0), _port(0), _clientSocket(0), \
    _serverAddress(NULL), _clientAddress(NULL), _sockaddLen(0), _requestLength(0), \
    _buff(NULL), _root(".") { }

Webserver::Webserver( const Webserver &rhs ) : _socket(rhs._socket), _port(rhs._port), _clientSocket(rhs._clientSocket), \
    _buff(NULL)
{
    this->_serverAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    this->_clientAddress = new sockadd;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
    this->_requestLength = rhs._requestLength;
	this->_root = ".";
}

Webserver &Webserver::operator=( const Webserver &rhs )
{
    this->_socket = rhs._socket;
    this->_port = rhs._port;
    this->_clientSocket = rhs._clientSocket;
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
    this->_requestLength = rhs._requestLength;
    this->_buff = NULL;
	this->_root = ".";
    return *this;
}

Webserver::Webserver( const int portToUse, const size_t reqLen ) : _port(portToUse), _requestLength(reqLen)
{
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    this->_clientSocket = 0;
    this->_serverAddress->sin_family = PF_INET;
    this->_serverAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    this->_serverAddress->sin_port = htons(this->_port);
    this->_sockaddLen = sizeof(sockaddr_in);
    this->_buff = NULL;
	this->_root = ".";
}

Webserver::~Webserver( void )
{
    delete _serverAddress;
    delete _clientAddress;
}

int Webserver::getSocket( void ) const
{
    return this->_socket;
}

void Webserver::createSocket( void )
{
    this->_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (this->_socket == -1)
        throw "Socket function failed";
}

void Webserver::bindAndListen( void )
{
	int tmpVar;

    tmpVar = 1;
    if (this->_socket == 0)
    {
		throw "You still didn't create a socket";
    	return ;
    }
    if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &tmpVar, sizeof(int)) == -1)
        throw "Setsockopt function failed";
    if (bind(this->_socket, (struct sockaddr *)this->_serverAddress, this->_sockaddLen) == -1)
        throw "Bind function failed";
    if (listen(this->_socket, 2) == -1)
        throw "Listen functions failed";
}

void Webserver::acceptConnections( void )
{
    this->_clientSocket = accept(this->_socket, (struct sockaddr *)this->_clientAddress, &this->_sockaddLen);
    if (this->_clientSocket == -1)
        throw "Accept function failed";
    if (this->_buff)
        delete this->_buff;
    this->_buff = new char[this->_requestLength];
    if (recv(this->_clientSocket, this->_buff, this->_requestLength, 0) == -1)
    {
        delete this->_buff;
        throw "Recv function failed";
    }
    // this->handleRequest(buff, this->_clientSocket);
    // close(this->_clientSocket);
}

void Webserver::parseRequest( void )
{
	std::string request(this->_buff);
	std::string requestLine;

	requestLine = request.substr(0, request.find('\r'));
	this->_request.method = requestLine.substr(0, requestLine.find(' '));

	requestLine.erase(0, this->_request.method.length() + 1);
	this->_request.file = requestLine.substr(0, requestLine.find(' '));
	
	requestLine.erase(0, this->_request.file.length() + 1);
	this->_request.version = requestLine.substr(0, requestLine.length());
}

void Webserver::processRequest( void )
{
 	if (!(this->_request.method == "GET" || this->_request.method == "POST"
		|| this->_request.method == "DELETE"))
		send(this->_clientSocket, "HTTP/1.1 400 Bad Request\r\n\r\n<h1>This method is not supported</h1>", 66, 0);
	else if (this->_request.method == "GET")
        this->_get();
    else if (this->_request.method == "POST")
        this->_post();
    else if (this->_request.method == "DELETE")
        this->_delete();
}

// std::string Webserver::getIndexFile( void ) const
// {
//     std::ifstream file;
// 	std::stringstream s;

// 	file.open("index.html");
// 	if (!file.is_open())
// 		throw "File did not open";
// 	s << file.rdbuf();
//     return s.str();
// }

std::string Webserver::fileToString( const std::string &file ) const
{
    std::ifstream toOpen(file, std::ios::binary);
	std::string toReturn((std::istreambuf_iterator<char>(toOpen)), (std::istreambuf_iterator<char>()));

	return  toReturn;
}

void Webserver::_get( void ) const
{
	std::string response;

    if (this->_request.file == "/")
    {
		response = "HTTP/1.1 200 OK\r\n\r\n";
		response += fileToString("index.html");
		send(this->_clientSocket, response.c_str(), response.length(), 0);
    }
    else
    {
		if (this->_fileExists(this->_root + this->_request.file))
		{
			response = "HTTP/1.1 200 OK\r\n\r\n";
			response += fileToString(this->_root + this->_request.file);
			send(this->_clientSocket, response.c_str(), response.length(), 0);
		}
		else
		{
			std::cout << "File does not exist" << std::endl;
			response = "HTTP/1.1 404 Not Found\r\n\r\n <h1>File Not Found</h1>";
			send(this->_clientSocket, response.c_str(), response.length(), 0);
		}
    }
	close(this->_clientSocket);
}

void Webserver::_post( void ) const
{
    std::cout << "Post method not supported yet" << std::endl;
}

void Webserver::_delete( void ) const
{
    std::cout << "Post method not supported yet" << std::endl;
}

bool Webserver::_fileExists( const std::string &file ) const 
{
	std::ifstream toOpen(file);
	
	if (!toOpen.is_open())
		return false;
	toOpen.close();
	return true;
}