#pragma once

#include <iostream>
#include <map>
#include <ctype.h>
#include <utility>

class Request
{
	// Private member attributes
	// Should consider returning them back to private
	public :
		std::string _method;
		std::string _uri;
		std::string _version;
		std::string _queryStr;
		std::map< std::string, std::string > _headers;
		std::string _body;

	// Default constructor, copy constructor, assignment overload, destructor
	public :
		Request( void );
		Request( const Request &rhs );
		Request &operator=( const Request &rhs );
		~Request( void );

	// Accessors
	public :
		void setMethod( const std::string &meth );
		void setUri( const std::string &uri );
		void setVersion( const std::string &version );
		void setQueryString( const std::string &qStr );
		void insertHeader( const std::pair< std::string, std::string > &pair );
		void setBody( const std::string &body );

	// Member functions
	public :
		bool isSupported( void ) const;
		bool hasGoodSize( void ) const;
		bool hasAllowedChars( void ) const;
		bool isGoodVersion( void ) const;
		bool isRequestLineParsed( void ) const;
};