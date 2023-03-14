#include "MimeTypes.class.hpp"

MimeTypes::MimeTypes( void )
{
	std::pair< std::string, std::string > allTypes[13] =
	{
		std::make_pair("audio/aac", ".aac"),
		std::make_pair("application/x-abiword", ".abw"),
		std::make_pair(".arc", "application/x-freearc"),
		std::make_pair(".avif", "image/avif"),
		std::make_pair(".avi", "video/x-msvideo"),
		std::make_pair(".azw", "application/vnd.amazon.ebook"),
		std::make_pair(".bin", "application/octet-stream"),
		std::make_pair(".bmp", "image/bmp"),
		std::make_pair(".bz", "application/x-bzip"),
		std::make_pair(".bz2", "application/x-bzip2"),
		std::make_pair(".cda", "application/x-cdf"),
		std::make_pair(".csh", "application/x-csh"),
		std::make_pair(".css", "text/css")
	};
	this->types.insert(allTypes, allTypes + 12);
}

MimeTypes::MimeTypes( const MimeTypes &rhs ) { *this = rhs; }

MimeTypes &MimeTypes::operator=( const MimeTypes &rhs )
{
	this->types = rhs.types;
	return *this;
}

MimeTypes::~MimeTypes( void ) { }