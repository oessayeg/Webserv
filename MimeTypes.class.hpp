#pragma once
#include <map>
#include <utility>
#include <iostream>

class MimeTypes
{
	private :
		std::map< std::string, std::string > types;

	public :
		MimeTypes( void );
		MimeTypes( const MimeTypes &rhs );
		MimeTypes &operator=( const MimeTypes &rhs );
		~MimeTypes( void );

		std::string getExtension( std::string contentType );
};