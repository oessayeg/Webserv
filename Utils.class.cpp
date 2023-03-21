#include "Utils.class.hpp"

Utils::Utils( void ) { }

Utils::~Utils( void ) { }


void Utils::setErrorResponse( int code, const std::string &s1, const std::string &s2, Client &client )
{
	client.clientResponse.setResponse(client.formError(code, s1, s2));
	client.clientResponse.setBool(true);
	client.typeCheck = POLLOUT;
}

void Utils::setGoodResponse( const std::string &s, Client &client )
{
	client.clientResponse.setResponse(s);
	client.clientResponse.setBool(true);
	client.typeCheck = POLLOUT;
}

std::string Utils::generateRandomString( void )
{
    std::string tmp_s;
    const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

	srand(time(NULL));
    tmp_s.reserve(8);
    for (int i = 0; i < 8; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

size_t Utils::giveDecimal( const std::string &hexaString )
{
	std::stringstream ss;
	size_t ret;

	ss << std::hex << hexaString;
	ss >> ret;
	return ret;
}

std::string	Utils::getSizeOfFile( std::ifstream &file )
{
	std::stringstream size;

	file.seekg(0, std::ios::end);
	int file_size = file.tellg();
	file.seekg(0, std::ios::beg);
	size << file_size;
	return (size.str());
}

std::string Utils::getPathInfo( void )
{
	char currentPath[FILENAME_MAX];

	if(getcwd(currentPath, sizeof(currentPath)) != NULL)
		return (std::string(currentPath));
	return (NULL);
}

std::string Utils::getFileContent( std::ifstream &file )
{
	std::stringstream buffer;

	buffer << file.rdbuf();
	return (buffer.str());
}

std::string	Utils::handleAutoindexFolder(const std::string &uri)
{
	DIR *dir;
	struct dirent *folder;
	std::stringstream response;

	response << "<html><body><ul>" <<std::endl;
	if((dir = opendir(uri.c_str())) != NULL)
	{
		while((folder = readdir (dir)) != NULL)
			if(folder->d_name[0] != '.')
				response << "<li><a href=\"" << folder->d_name << "\">" << folder->d_name << "</a></li>\n";
		closedir(dir);
	}
	response <<"</ul></body></html>"<<std::endl;
	return (response.str());	
}