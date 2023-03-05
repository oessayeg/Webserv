#pragma once

#include <iostream>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <utility>

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
		std::map< int, std::string > errorMap;

	public :
		Blocks( void );
		Blocks( const Blocks &rhs );
		Blocks &operator=( const Blocks &rhs );
		~Blocks( void );
};