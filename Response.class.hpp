#pragma once

#include <iostream>
#include <sys/socket.h>
#include <fstream>

class Response
{
	public :
		std::ifstream file;
		std::string _nameOfFile;
		std::string _response;
		std::string _status;
		bool _canBeSent;
		bool _shouldReadFromFile;
		bool _isStatusSent;
		size_t r;

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