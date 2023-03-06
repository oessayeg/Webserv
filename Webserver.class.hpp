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
		void _readRequest( Client &client );
		void _parseRequestLine( Client &client );
		void _parseHeaders( Client &client );
		void _readBodyIfPossible( Client &client );
		void _prepareResponse( Client &client );
		void _prepareGetResponse( Client &client );
		// void _preparePostResponse( Client &client );
		// void _prepareDeleteResponse( Client &client );
};