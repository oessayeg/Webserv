#pragma once

#include "MainHeader.hpp"
class Blocks;


class Client
{
	private :
		int _socket;

	public :
		int bytesRead;
		char *response;
		char request[MAX_RQ];
		Request parsedRequest;
		std::string stringRequest;
		Blocks *correspondingBlock;
		struct sockaddr_in *clientStruct;
		bool isRead;

	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );
		void setSocket( int s );
		int getSocket( void );
		void read( void );
		void parseRequest( void );
	
	// public :

};