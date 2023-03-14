#pragma once

#include "Block.tmp.hpp"
#include "MainHeader.hpp"
#include "ErrorString.class.hpp"

class Client
{
	private :
		int _socket;

	public :
		int bytesRead;
		char request[MAX_RQ + 1];
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;
		bool shouldReadBody;
		bool finishedBody;
		bool gotFileName;
		bool shouldSkip;
		size_t bytesToRead;
		struct sockaddr_in *clientStruct;
		std::ofstream fileToUpload;
		std::string stringRequest;
		std::string boundary;
		std::string body;
		ErrorString errString;
		int bodyType;
		size_t bytesCounter;

		// These are the three attributes that you need
		Blocks *correspondingBlock;
		Response clientResponse;
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
		void checkBody( const std::string &key, const std::string &value );

		// This function forms the correct error response in case of an error
		std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody );

		// Functions to parse the body in case of POST method
		bool isThereFilename( int bodyType );
		void parseMultipartBody( void );
		// void parseChunkedMultipart( void );

		// Utils
		// Give decimal should be optimized
		size_t giveDecimal( const std::string &hexaString );
		bool isEndOfBody( void );
		void openFile( char *name );
		char *giveBody( char *limiter );
		char *giveDelimiter( void );
		bool isBoundary( char *ptr );
		void parseChunkedMultipart( void );
		void parseChunkedBody( void );
		void parseNormalData( void );
		std::string generateRandomString( void );
};