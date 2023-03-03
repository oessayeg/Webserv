#pragma once

#include <iostream>
#include <netinet/in.h>
#include <vector>

class Blocks
{
	public :
		int port;
		int ip;
		int listeningSocket;
		int maxBodySize;
		std::vector < std::string > index;
		std::string root;
		struct sockaddr_in socketNeeds;
	
	public :
		Blocks( void );
		Blocks( const Blocks &rhs );
		Blocks &operator=( const Blocks &rhs );
		~Blocks( void );
};