#pragma once

#include "MainHeader.hpp"
#include "Configfile.hpp"

typedef struct pollfd pollfd;
typedef struct sockaddr_in sockaddr_in;

class Webserver
{
	// 1st list contains instructions in server blocks from the configfile.
	// 2nd list contains all clients connected and waiting for a response.
	// 3rd list contains listening sockets that'll accept new connections.
	// 4th attribute is the struct pointer that'll be sent to poll() to check for I/O operations.
	// 5th attribute is an object that will parse bodies in case of POST requests.
	private :
		std::list < Serverblock > _serverBlocks;
		std::list < Client > _pendingClients;
		std::list < int > _listeningSockets;
		pollfd *_fdToCheck;
		BodyParser _parser;

	// Constructors, '=' overload and destructor (Coplien's form).
	public :
		Webserver( void );
		Webserver( std::list < Serverblock > &rhs );
		Webserver( const Webserver &rhs );
		Webserver &operator=( const Webserver &rhs );
		~Webserver( void );

		// Functions that prepare for the multiplexing part.
		void setServerBlocks( std::list < Serverblock > &list );
		void createSockets( void );
		void setReadyFds( void );
		
		// Main multiplexing function
		void readAndRespond( void );

	// Description of these functions in 'Webserver.class.cpp'.
	private :
		void _acceptNewClients( void );
		void _readAndParse( Client &client );
		void _readRequest( Client &client );
		void _parseRequestLine( Client &client );
		void _parseHeaders( Client &client );
		void _readBodyIfPossible( Client &client );
		void _dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend );
		void _prepareResponse( Client &client );
		void _handleProperResponse( Client &client );
		void _prepareGetResponse( Client &client );
		void _preparePostResponse( Client &client );
		void _prepareDeleteResponse( Client &client );
		void _handleHttpRedirection( std::list< Location >::iterator &currentList, Client &client );
		void _runCgi( std::string &nameFile, Client & );
		void _readFile( std::string path, Client &client, std::string &name );
		void _removeContent( const std::string &, Client &, int &, bool &shouldPrint );
		void _handleFolderRequest( Client &client );
		void _handleFileRequest( Client &client );
		void _handleDeleteFolderRequest( Client &client );
		void _handleDeleteFile( Client &client );
		bool _sendFile( std::list< Client >::iterator &it );
		bool _sendWithStatusCode( std::list< Client >::iterator &it, int bytes, char *buff );
		char **_prepareCgiEnv( Client &client, std::string &name );
		char **_prepareArgs( const std::string &name );
};