#pragma once

#include "Client.class.hpp"

class Client;

class Utils
{
	public :
		Utils( void );
		~Utils( void );

	public :
		static void setErrorResponse( int code, const std::string &s1, const std::string &s2, Client &client );
		static void setGoodResponse( const std::string &s, Client &client );
};