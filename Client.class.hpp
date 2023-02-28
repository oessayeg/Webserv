#pragma once

#include "MainHeader.hpp"
class Blocks;


class Client
{
	private :
		int _socket;
		int _bytesRead;
		char *_response;
		char _request[MAX_RQ];
		struct sockaddr_in *_clientStruct;

	public :
		Request _parsedRequest;
		Blocks *correspondingBlock;

	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );
	
	public :
		void setSocket( int s );
		int getSocket( void ) const;
		struct sockaddr_in *getClientStruct( void );
};