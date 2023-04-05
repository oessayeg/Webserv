#pragma once

#include "Client.class.hpp"
#include "Utils.class.hpp"

class Client;

class BodyParser
{
	private :
		MimeTypes _extensions;

	// Constructors, '=' overload and destructor.
	public :
		BodyParser( void );
		BodyParser( const BodyParser &rhs );
		BodyParser &operator=( const BodyParser &rhs );
		~BodyParser( void );
	
	// Public member functions (each one is used depending on the type of the body to be parsed).
	public :
		void chooseCorrectParsingMode( Client &client );
		void parseMultipartData( Client &client );
		void parseChunkedData( Client &client );
		void parseNormalData( Client &client );
		std::string getContentType( const std::string & );

	// Helpers (description in their implementation file 'BodyParser.class.cpp').
	private :
		void _openWithProperExtension( const std::string &contentType, Client &client );
		void _openFile( char *name, Client &client );
		void _moveRequest( size_t index2, Client &client );
		bool _isThereFilename( Client &client );
		bool _isBoundary( char *ptr, Client &client );
		bool _isHexaReadable( Client &client );
};