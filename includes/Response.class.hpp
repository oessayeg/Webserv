#pragma once

#include <iostream>
#include <sys/socket.h>
#include <fstream>

class Response
{
	private :
		bool _canBeSent;
		bool _shouldReadFromFile;
		bool _isStatusSent;
		size_t _fileSize;
		
	public :
		std::ifstream file;
		std::string _nameOfFile;
		std::string _response;
		std::string _status;
		size_t r;

	public :
		Response( void );
		Response( const Response &rhs );
		Response &operator=( const Response &rhs );
		~Response( void );

	public :
		bool getBool( void ) const;
		bool readFromFile( void ) const;
		bool getIsStatusSent( void ) const;
		void setBool( bool isReady );
		void setReadFromFile( bool shouldRead );
		void setIsStatusSent( bool isItSent );
		void setFileSize( size_t size );
		void sendResponse( int socket ) const;
		void setResponse( const std::string &resp );
		size_t getFileSize( void ) const;
};