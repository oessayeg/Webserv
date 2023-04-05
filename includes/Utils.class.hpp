#pragma once

#include "Client.class.hpp"

class Client;

// This class contains static functions that can be used without creating an object of it.
class Utils
{
	public :
		Utils( void );
		~Utils( void );

	// Description of these functions ins 'Utils.class.cpp'.
	public :
		static void setErrorResponse( int code, const std::string &s1, const std::string &s2, Client &client );
		static void setGoodResponse( const std::string &s, Client &client );
		static void deleteDoublePtr( char **toDelete );
		static void checkRequestLine( Client &client );
		static void checkArgs( int ac, char **av );
		static void checkHeaders( Client &client );
		static void setType( const std::string &transferEnc, const std::string &contentType, Client &client );

		static bool serverNameMatches( const std::string &host, Serverblock *block );
		static bool isAccepted( const std::string &method, std::list< std::string > list );
		static bool isLocationFormedWell( const std::string &transferEnc, Client &client );

		static std::string getFileN( char **av );
		static std::string getSizeInString( const std::string &str );
		static std::string generateRandomString( void );
		static std::string getSizeOfFile( const std::string &file );
		static std::string getPathInfo( void );
		static std::string getFileContent( std::ifstream &file );
		static std::string handleAutoindexFolder( const std::string &uri );
		static std::string formError( int statusCode, const std::string &statusLine, const std::string &msgInBody, Client &client );
		static std::string getIndex( Client &client );
		static size_t giveDecimal( const std::string &hexaString );
		static size_t getSize( const std::string &file);

		static char *giveAllocatedChar( const std::string &str );
};