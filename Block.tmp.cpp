#include "Block.tmp.hpp"

Blocks::Blocks( void )
{
	this->port = 0;
	this->ip = 0;
	this->listeningSocket = 0;
}

Blocks::Blocks( const Blocks &rhs )
{
	*this = rhs;
}

Blocks &Blocks::operator=( const Blocks &rhs )
{
	if (this != &rhs)
	{
		this->port = rhs.port;
		this->ip = rhs.ip;
		this->listeningSocket = rhs.listeningSocket;
		this->index = rhs.index;
		this->root = rhs.root;
		this->socketNeeds = rhs.socketNeeds;
	}
	return *this;
}

Blocks::~Blocks( void ) { }