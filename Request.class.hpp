#pragma once

#include <iostream>
#include <unordered_map>
#include <ctype.h>
#include <utility>

class Request
{
	private :
		std::string _method;
		std::string _uri;
		std::string _version;
		std::unordered_map< std::string, std::string > _headers;
		std::string _body;

	public :
		void setMethod( const std::string &meth );
		void setUri( const std::string &uri );
		void setVersion( const std::string &version );
		void insertHeader( const std::pair< std::string, std::string > &pair );
		void setBody( const std::string &body );

	public :
		bool isSupported( void ) const;
		bool hasGoodSize( void ) const;
		bool hasAllowedChars( void ) const;
		bool isGoodVersion( void ) const;
		bool isRequestLineParsed( void ) const;
};