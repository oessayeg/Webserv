#pragma once

#include "Block.tmp.hpp"
#include "MainHeader.hpp"

class Client
{
	private :
		int _socket;

	public :
		int bytesRead;
		char *response;
		char request[MAX_RQ];
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;
		struct sockaddr_in *clientStruct;
		Blocks *correspondingBlock;
		std::string stringRequest;
		Response clientResponse;
		Request parsedRequest;

	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );
		void setSocket( int s );
		int getSocket( void );
		void checkRequestLine( void );
		void checkHeaders( void );
};