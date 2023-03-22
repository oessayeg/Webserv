#pragma once

#include <iostream>
#include <sys/socket.h>

class Response
{
	public :
		std::string _nameOfFile;
		std::string _response;
		bool _canBeSent;
		bool _shouldReadFromFile;

	public :
		Response( void );
		Response( const Response &rhs );
		Response &operator=( const Response &rhs );
		~Response( void );

	public :
		bool getBool( void ) const;
		void setBool( bool isReady );
		void sendResponse( int socket ) const;
		void setResponse( const std::string &resp );
};