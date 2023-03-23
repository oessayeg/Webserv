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
		void setServerBlocks( std::list < Serverblock > &list );
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
		void _dropClient( std::list< Client >::iterator &it, bool *inc, bool shouldSend );
		void _prepareResponse( Client &client );
		void _handleProperResponse( Client &client );
		void _prepareGetResponse( Client &client );
		void _preparePostResponse( Client &client );
		void _prepareDeleteResponse( Client &client );
		void _handleHttpRedirection( std::list< Location >::iterator &currentList, Client &client );
		void _handleCgi( std::list< Location>::iterator &currentList,  Client &client, const std::string &root );
		void _runCgi( std::string &nameFile, Client & );
		void _readFile( std::string &path, Client &client, std::string &name );
		void _removeContent( const std::string &, Client &, int &, bool &shouldPrint );
		void _handleDeleteFolderRequest( Client &client );
		void _handleDeleteFile( Client &client );
		void _handleFolderRequest( Client & );
		void _handleFileRequest( Client & );
		bool _sendFile( std::list< Client >::iterator &it );
		bool _sendWithStatusCode( std::list< Client >::iterator &it, int bytes, char *buff );
};