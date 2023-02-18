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
        if (clientSocket == -1)
            throw "Accept function failed";
        buff = new char[this->_requestLength];
        if (recv(clientSocket, buff, this->_requestLength, 0) == -1)
        {
            delete buff;
            throw "Recv function failed";
        }
        this->handleRequest(std::string(buff));
        delete buff;
        close(clientSocket);
    }
}

void Webserver::handleRequest( const std::string &req )
{
	std::cout << "Need to handle request" << std::endl;
}