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
		static bool serverNameMatches( const std::string &host, Serverblock *block );
		static std::string getFileN( char **av );
		static void checkArgs( int ac, char **av );
		static std::string getSizeInString( const std::string &str );
		static bool isAccepted( const std::string &method, std::list< std::string > list );
		static void checkRequestLine( Client &client );
		static void checkHeaders( Client &client );
		static void setType( const std::string &transferEnc, const std::string &contentType, Client &client );
		static std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody, Client &client );
		static bool isLocationFormedWell( const std::string &transferEnc, Client &client );
};