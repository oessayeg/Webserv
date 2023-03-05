#pragma once

#include <iostream>

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
		void setErrorFile( const std::string &errorMessage );
};