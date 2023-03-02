#include "Response.class.hpp"


Response::Response( void ) { }

Response::Response( const Response &rhs )
{
	*this = rhs;
}

Response &Response::operator=( const Response &rhs )
{
	// Here, should not forget to init vars with rhs vars
	return *this;
}

Response::~Response( void ) { }