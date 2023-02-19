#include "Webserver.hpp"

// Network byte order is big-endian, so htons make sure that our nbr is big-endian \
// if our machine stores nbrs in little endian format

Webserver::Webserver( void ) : _socket(0), _port(0), _serverAddress(NULL), \
    _clientAddress(NULL), _sockaddLen(0), _requestLength(0) { }

Webserver::Webserver( const Webserver &rhs ) : _socket(rhs._socket), _port(rhs._port)
{
    this->_serverAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    this->_clientAddress = new sockadd;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
    this->_requestLength = rhs._requestLength;
}

Webserver &Webserver::operator=( const Webserver &rhs )
{
    this->_socket = rhs._socket;
    this->_port = rhs._port;
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    *this->_serverAddress = *rhs._serverAddress;
    *this->_clientAddress = *rhs._clientAddress;
    this->_sockaddLen = rhs._sockaddLen;
    this->_requestLength = rhs._requestLength;
    return *this;
}

Webserver::Webserver( const int portToUse, const size_t reqLen ) : _port(portToUse), _requestLength(reqLen)
{
    this->_serverAddress = new sockadd;
    this->_clientAddress = new sockadd;
    this->_serverAddress->sin_family = PF_INET;
    this->_serverAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    this->_serverAddress->sin_port = htons(this->_port);
    this->_sockaddLen = sizeof(sockaddr_in);
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
    int clientSocket;
    char *buff;
    
    clientSocket = 0;
    while (1)
    {
        clientSocket = accept(this->_socket, (struct sockaddr *)this->_clientAddress, &this->_sockaddLen);
		std::cout << "Client Socket = " << clientSocket << std::endl;
        if (clientSocket == -1)
            throw "Accept function failed";
        buff = new char[this->_requestLength];
        if (recv(clientSocket, buff, this->_requestLength, 0) == -1)
        {
            delete buff;
            throw "Recv function failed";
        }
        this->handleRequest(buff, clientSocket);
        delete buff;
        close(clientSocket);
    }
}

void Webserver::handleRequest( char *req, const int clientSock )
{
	std::string request(req);
	std::string requestLine;

	requestLine = request.substr(0, request.find('\r'));
	this->_request.method = requestLine.substr(0, requestLine.find(' '));

	requestLine.erase(0, this->_request.method.length() + 1);
	this->_request.file = requestLine.substr(0, requestLine.find(' '));
	
	requestLine.erase(0, this->_request.file.length() + 1);
	this->_request.version = requestLine.substr(0, requestLine.length());

	if (!(this->_request.method == "GET" || this->_request.method == "POST"
		|| this->_request.method == "DELETE"))
		send(clientSock, "HTTP/1.1 400 Not Found\r\n\r\n<h1>This method is not supported</h1>", 63, 0);
	else if (this->_request.method == "GET" && this->_request.file == "/")
    {
		std::string response("HTTP/1.1 200 OK\r\n\r\n" + getIndexFile());
		send(clientSock, response.c_str(), response.length(), 0);
    }
}

std::string Webserver::getIndexFile( void ) const
{
    std::ifstream file;
	std::stringstream s;

	file.open("index.html");
	if (!file.is_open())
		throw "File did not open";
	s << file.rdbuf();
    return s.str();
}