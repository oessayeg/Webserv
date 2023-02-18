#pragma once

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <poll.h>
#include <map>

typedef struct sockaddr_in sockadd;

class Webserver
{
    private :
        int _socket;
        int _port;
        sockadd *_serverAddress;
        sockadd *_clientAddress;
        socklen_t _sockaddLen;

    public :
        Webserver( void );
        Webserver( const Webserver &rhs );
        Webserver &operator=( const Webserver &rhs );
        Webserver( const int port );
        ~Webserver( void );
        
        // Getters
        int getSocket( void ) const;

        // Socket handlers
        void createSocket( void );
};