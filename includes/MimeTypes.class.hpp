#pragma once
#include <map>
#include <utility>
#include <iostream>

class MimeTypes
{
	// This map contains all common MiMe types.
	private :
		std::map< std::string, std::string > types;

	// Constructors, '=' overload and destructor.
	public :
		MimeTypes( void );
		MimeTypes( const MimeTypes &rhs );
		MimeTypes &operator=( const MimeTypes &rhs );
		~MimeTypes( void );
	
	// These public member functions get the extension or content-type from the map.
	public :
		std::string	getContentType(const std::string &path);
		std::string getExtension( const std::string &contentType );
};