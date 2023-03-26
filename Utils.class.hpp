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
		static std::string generateRandomString( void );
		static size_t giveDecimal( const std::string &hexaString );
		static size_t getSize( const std::string &file);
		static std::string getSizeOfFile( const std::string &file );
		static std::string getPathInfo( void );
		static std::string getFileContent( std::ifstream &file );
		static std::string handleAutoindexFolder( const std::string &uri );
		static char *giveAllocatedChar( const std::string &str );
		static void deleteDoublePtr( char **toDelete );
};