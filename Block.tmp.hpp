#pragma once

#include "MainHeader.hpp"

class Blocks
{
	public :
		int port;
		int ip;
		int listeningSocket;
		std::vector < std::string > index;
		std::string root;
		struct sockaddr_in socketNeeds;
	
	public :
		Blocks( void );
		Blocks( const Blocks &rhs );
		Blocks &operator=( const Blocks &rhs );
		~Blocks( void );
};