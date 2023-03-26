#pragma once

#include <iostream>
#include <sstream>

class ErrorString
{
	private :
		std::string _errorFile;

	public :
		ErrorString( void );
		ErrorString( const ErrorString &rhs );
		ErrorString &operator=( const ErrorString &rhs );
		~ErrorString( void );
	
	public :
		std::string getFileInString( void ) const;
		void setErrorFile( int sCode, const std::string &errorMessage );
};