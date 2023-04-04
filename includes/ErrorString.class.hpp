#pragma once

#include <iostream>
#include <sstream>

class ErrorString
{
	// This attribute contains html/css code for default error pages
	private :
		std::string _errorFile;

	// Constructors, '=' overload and destructor
	public :
		ErrorString( void );
		ErrorString( const ErrorString &rhs );
		ErrorString &operator=( const ErrorString &rhs );
		~ErrorString( void );
	
	// Description in (ErrorString.class.cpp)
	public :
		std::string getFileInString( void ) const;
		void setErrorFile( int sCode, const std::string &errorMessage );
};