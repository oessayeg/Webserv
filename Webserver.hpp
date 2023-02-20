#pragma once

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iterator>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <poll.h>
#include <map>

// To remove and add to main header file
typedef struct sockaddr_in sockadd;
typedef struct request
{
    std::string method;
    std::string file;
    std::string version;
} clientRequest;

class Webserver
{
    private :
        int _socket;
        int _port;
        int _clientSocket;
        sockadd *_serverAddress;
        sockadd *_clientAddress;
        socklen_t _sockaddLen;
        size_t _requestLength;
        clientRequest _request;
        char *_buff;
        std::string _root;

    public :
        Webserver( void );
        Webserver( const Webserver &rhs );
        Webserver &operator=( const Webserver &rhs );
        Webserver( const int port, const size_t reqLen );
        ~Webserver( void );

        // Getters
        int getSocket( void ) const;

        // Socket handlers
        void createSocket( void );
        void bindAndListen( void );
        void acceptConnections( void );
        void parseRequest( void );
        void processRequest( void );
        std::string fileToString( const std::string &file ) const;

    // Private member functions
    private :
        void _get( void ) const;
        void _post( void ) const;
        void _delete( void ) const;
        bool _fileExists( const std::string &file ) const;
};