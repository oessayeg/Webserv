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
		char request[MAX_RQ];
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;
		bool isThereBody;
		bool finishedBody;
		struct sockaddr_in *clientStruct;
		Blocks *correspondingBlock;
		std::string stringRequest;
		std::string body;
		Response clientResponse;
		Request parsedRequest;
		ErrorString errString;

	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );
		void setSocket( int s );
		int getSocket( void );
		void checkRequestLine( void );
		void checkHeaders( void );
		std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody );
};