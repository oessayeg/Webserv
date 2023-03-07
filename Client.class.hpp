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
		struct sockaddr_in *clientStruct;
		std::string bodyDelimiter;
		std::string stringRequest;
		std::string body;
		ErrorString errString;

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

		// These are checkFunctions that check if the request or header are well formed
		void checkRequestLine( void );
		void checkHeaders( void );
		void checkBody( const std::string &key, const std::string &value );

		// This function forms the correct error response in case of an error
		std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody );
};