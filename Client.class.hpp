#pragma once

#include "MainHeader.hpp"
// #include "Response.class.hpp"

class Blocks;

class Client
{
	private :
		int _socket;

	public :
		Request parsedRequest;
		Response clientResponse;
		int bytesRead;
		char *response;
		char request[MAX_RQ];
		std::string stringRequest;
		struct sockaddr_in *clientStruct;
		Blocks *correspondingBlock;
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;

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