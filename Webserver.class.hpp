#pragma once

#include "MainHeader.hpp"

typedef struct Blocks Blocks;
typedef struct pollfd pollfd;
typedef struct sockaddr_in sockaddr_in;

class Webserver
{
	private :
		std::list < Blocks > _serverBlocks;
		std::list < Client > _pendingClients;
		std::list < int > _listeningSockets;
		pollfd *_fdToCheck;

	public :
		// Webserver Constructors
		Webserver( void );
		Webserver( const Webserver &rhs );
		Webserver &operator=( const Webserver &rhs );
		Webserver( std::list < Blocks > &rhs );
		~Webserver( void );

		// Need to change the list param to a reference
		void setServerBlocks( std::list < Blocks > list );
		void createSockets( void );
		void setReadyFds( void );
		void readAndRespond( void );

	private :
		void _acceptNewClients( void );
		// void _readRequest( std::list< clients * >::iterator client );
		// void _sendResponse( std::list< clients * >::iterator client );
		// bool _parseRequest( std::list< clients * >::iterator client );

	public : // Temporary status code functions
		// void send405( std::list< clients >::iterator client );
		// void send414( std::list< clients >::iterator client );
		// void send400( std::list< clients >::iterator client );
		// void send505( std::list< clients >::iterator client );

};