#pragma once

#include "MainHeader.hpp"
#include "ParseConfigfile/Configfile.hpp"

typedef struct pollfd pollfd;
typedef struct sockaddr_in sockaddr_in;

class Webserver
{
	private :
		std::list < Serverblock > _serverBlocks;
		std::list < Client > _pendingClients;
		std::list < int > _listeningSockets;
		pollfd *_fdToCheck;
		BodyParser parser;

	public :
		// Webserver Constructors
		Webserver( void );
		Webserver( const Webserver &rhs );
		Webserver &operator=( const Webserver &rhs );
		Webserver( std::list < Serverblock > &rhs );
		~Webserver( void );

		// Need to change the list param to a reference
		void setServerBlocks( std::list < Serverblock > list );
		void createSockets( void );
		void setReadyFds( void );
		void readAndRespond( void );

	private :
		void _acceptNewClients( void );
		void _readAndParse( Client &client );
		void _readRequest( Client &client );
		void _parseRequestLine( Client &client );
		void _parseHeaders( Client &client );
		void _readBodyIfPossible( Client &client );
		void _prepareResponse( Client &client );
		void _prepareGetResponse( Client &client );
		void _dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend );
		std::list<Location>::iterator	ifUriMatchLocationBlock(std::list<Location> &list, const std::string &uri);

		// void _preparePostResponse( Client &client );
		// void _prepareDeleteResponse( Client &client );
};