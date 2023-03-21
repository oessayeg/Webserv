#include "Utils.class.hpp"

Utils::Utils( void ) { }

Utils::~Utils( void ) { }


void Utils::setErrorResponse( int code, const std::string &s1, const std::string &s2, Client &client )
{
	client.clientResponse.setResponse(client.formError(code, s1, s2));
	client.clientResponse.setBool(true);
	client.typeCheck = POLLOUT;
}

void Utils::setGoodResponse( const std::string &s, Client &client )
{
	client.clientResponse.setResponse(s);
	client.clientResponse.setBool(true);
	client.typeCheck = POLLOUT;
}