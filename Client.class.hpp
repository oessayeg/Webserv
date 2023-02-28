#pragma once

#include "MainHeader.hpp"

class Client
{
	private :
		int _port;
		int _socket;
		int _bytesRead;
		char *_response;
		char _request[MAX_RQ];
		Request _parsedRequest;
		struct sockaddr_in *_clientStruct;
	
	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );
};