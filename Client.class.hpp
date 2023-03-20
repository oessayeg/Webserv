#pragma once

#include "MainHeader.hpp"
#include "ErrorString.class.hpp"

class Client
{
	private :
		int _socket;

	public :
		struct sockaddr_in *clientStruct;
		char request[BUFF_SIZE + 1];
		int bodyType;
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;
		bool shouldReadBody;
		bool finishedBody;
		bool gotFileName;
		bool shouldSkip;
		bool isConnected;
		bool isThereCgi;
		short typeCheck;
		size_t bytesRead;
		size_t bytesToRead;
		size_t bytesCounter;
		size_t contentLength;
		std::ofstream fileToUpload;
		std::string stringRequest;
		std::string boundary;
		std::string filePath;
		std::string nameForCgi;
		std::list< Location >::iterator currentList;

	public :
		Serverblock *correspondingBlock;
		Response clientResponse;
		ErrorString errString;
		Request parsedRequest;

	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );

	public :
		// Accessors for the _socket variable
		void setSocket( int s );
		int getSocket( void );

		// These are checkFunctions that check if the request or headers are well formed
		void checkRequestLine( void );
		void checkHeaders( void );
		void setType( std::string transferEnc, std::string contentType );
		void checkBody( const std::string &key, const std::string &value );

		// This function forms the correct error response in case of an error
		std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody );
		bool isLocationFormedWell( std::string &transferEnc );
};