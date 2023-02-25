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
		std::list < int > _listeningSockets;
		pollfd *_fds;

	public :
		// Webserver Constructors
		Webserver( void );
		Webserver( const Webserver &rhs );
		Webserver &operator=( const Webserver &rhs );
		Webserver( std::list < blocks > &rhs );
		~Webserver( void );

		void setServerBlocks( std::list < blocks > list );
		void createSockets( void );
		void setReadyFds( void );
		void readAndRespond( void );

	private :
		void _readRequest( std::list< clients >::iterator client );
		void _sendResponse( std::list< clients >::iterator client );
};