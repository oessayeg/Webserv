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
		size_t _bytesFromFile;
		std::string _response;
		
	public :
		std::string status;
		std::ifstream file;

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
		void incrementBytesFromFile( size_t b );
		size_t getFileSize( void ) const;
		size_t getBytesFromFile( void ) const;
};