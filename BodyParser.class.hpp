#pragma once

#include "Client.class.hpp"

class BodyParser
{
	private :
		MimeTypes _extensions;

	public :
		BodyParser( void );
		BodyParser( const BodyParser &rhs );
		BodyParser &operator=( const BodyParser &rhs );
		~BodyParser( void );
	
	public :
		void parseMultipartData( Client &client );
		void parseChunkedData( Client &client );
		void parseNormalData( Client &client );

	private :
		void _openWithProperExtension( const std::string &contentType, Client &client );
		void _openFile( char *name, Client &client );
		bool _isThereFilename( int bodyType, Client &client );
		bool _isBoundary( char *ptr, Client &client );
		size_t _giveDecimal( const std::string &hexaString );
		std::string _randomString( void );
};