#pragma once

#include "MainHeader.hpp"

typedef struct blocks blocks;
typedef struct clients clients;
typedef struct pollfd pollfd;

class Webserver
{
    private :
        std::list < blocks > _serverBlocks;
        std::list < clients > _pendingClients;
        pollfd *_fds;

    public :
        // Webserver Constructors
        Webserver( void );
        Webserver( const Webserver &rhs );
        Webserver &operator=( const Webserver &rhs );
        Webserver( std::list < blocks > &rhs );
        ~Webserver( void );

        // Set the server blocks to the list passed
        void setServerBlocks( std::list < blocks > &list );
};