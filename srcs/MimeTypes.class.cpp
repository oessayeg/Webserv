#include "../includes/MimeTypes.class.hpp"

MimeTypes::MimeTypes( void )
{
	std::pair< std::string, std::string > allTypes[76] =
	{
		std::make_pair("audio/aac", ".aac"),
		std::make_pair("application/x-abiword", ".abw"),
		std::make_pair("application/x-freearc", ".arc"),
		std::make_pair("image/avif", ".avif"),
		std::make_pair("video/x-msvideo", ".avi"),
		std::make_pair("application/vnd.amazon.ebook", ".azw"),
		std::make_pair("application/octet-stream", ".bin"),
		std::make_pair("image/bmp", ".bmp"),
		std::make_pair("application/x-bzip", ".bz"),
		std::make_pair("application/x-bzip2", ".bz2"),
		std::make_pair("application/x-cdf", ".cda"),
		std::make_pair("application/x-csh", ".csh"),
		std::make_pair("text/css", ".css"),
		std::make_pair("text/csv", ".csv"),
		std::make_pair("application/msword", ".doc"),
		std::make_pair("application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".docx"),
		std::make_pair("application/vnd.ms-fontobject", ".eot"),
		std::make_pair("application/epub+zip", ".epub"),
		std::make_pair("application/gzip", ".gz"),
		std::make_pair("image/gif", ".gif"),
		std::make_pair("text/html", ".html"),
		std::make_pair("image/vnd.microsoft.icon", ".ico"),
		std::make_pair("text/calendar", ".ics"),
		std::make_pair("application/java-archive", ".jar"),
		std::make_pair("image/jpeg", ".jpeg"),
		std::make_pair("text/javascript", ".js"),
		std::make_pair("application/json", ".json"),
		std::make_pair("application/ld+json", ".jsonld"),
		std::make_pair("audio/midi", ".mid"),
		std::make_pair("audio/x-midi", ".midi"),
		std::make_pair("text/javascript", ".mjs"),
		std::make_pair("audio/mpeg", ".mp3"),
		std::make_pair("video/mp4", ".mp4"),
		std::make_pair("video/mpeg", ".mpeg"),
		std::make_pair("application/vnd.apple.installer+xml", ".mpkg"),
		std::make_pair("application/vnd.oasis.opendocument.presentation", ".odp"),
		std::make_pair("application/vnd.oasis.opendocument.spreadsheet", ".ods"),
		std::make_pair("application/vnd.oasis.opendocument.text", ".odt"),
		std::make_pair("audio/ogg", ".oga"),
		std::make_pair("video/ogg", ".ogv"),
		std::make_pair("application/ogg", ".ogx"),
		std::make_pair("audio/opus", ".opus"),
		std::make_pair("font/otf", ".otf"),
		std::make_pair("image/png", ".png"),
		std::make_pair("application/pdf", ".pdf"),
		std::make_pair("application/x-httpd-php", ".php"),
		std::make_pair("application/vnd.ms-powerpoint", ".ppt"),
		std::make_pair("application/vnd.openxmlformats-officedocument.presentationml.presentation", ".pptx"),
		std::make_pair("application/vnd.rar", ".rar"),
		std::make_pair("application/rtf", ".rtf"),
		std::make_pair("application/x-sh", ".sh"),
		std::make_pair("image/svg+xml", ".svg"),
		std::make_pair("application/x-tar", ".tar"),
		std::make_pair("image/tiff", ".tif"),
		std::make_pair("video/mp2t", ".ts"),
		std::make_pair("font/ttf", ".ttf"),
		std::make_pair("text/plain", ".txt"),
		std::make_pair("application/vnd.visio", ".vsd"),
		std::make_pair("audio/wav", ".wav"),
		std::make_pair("audio/webm", ".weba"),
		std::make_pair("video/webm", ".webm"),
		std::make_pair("image/webp", ".webp"),
		std::make_pair("font/woff", ".woff"),
		std::make_pair("font/woff2", ".woff2"),
		std::make_pair("application/xhtml+xml", ".xhtml"),
		std::make_pair("application/vnd.ms-excel", ".xls"),
		std::make_pair("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", ".xlsx"),
		std::make_pair("application/xml", ".xml"),
		std::make_pair("text/xml", ".xml"),
		std::make_pair("application/vnd.mozilla.xul+xml", ".xul"),
		std::make_pair("application/zip", ".zip"),
		std::make_pair("video/3gpp", ".3gp"),
		std::make_pair("audio/3gpp", ".3gp"),
		std::make_pair("video/3gpp2", ".3g2"),
		std::make_pair("audio/3gpp2", ".3g2"),
		std::make_pair("application/x-7z-compressed", ".7z")
	};
	this->types.insert(allTypes, allTypes + 76);
}

MimeTypes::MimeTypes( const MimeTypes &rhs ) { *this = rhs; }

MimeTypes &MimeTypes::operator=( const MimeTypes &rhs )
{
	this->types = rhs.types;
	return (*this);
}

MimeTypes::~MimeTypes( void ) { }

std::string MimeTypes::getContentType( const std::string &path )
{
	size_t find = path.rfind(".");
	if(find == std::string::npos)
		return ("");
	std::string extension = path.substr(find);
	std::map<std::string, std::string>::iterator it  = types.begin();
	for(; it != types.end(); it++)
	{
		if(it->second == extension)
			return (it->first);
	}
	return ("");
}

std::string MimeTypes::getExtension( const std::string &contentType )
{
	return (types[contentType]);
}