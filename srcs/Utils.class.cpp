#include "../includes/Utils.class.hpp"

Utils::Utils( void ) { }

Utils::~Utils( void ) { }

// This function gets all the error response and puts it in a string.
// That response will be sent if the socket is ready for writing.
// It will also set a boolean to indicate that a response is ready. 
void Utils::setErrorResponse( int code, const std::string &s1, const std::string &s2, Client &client )
{
	client.clientResponse.setResponse(Utils::formError(code, s1, s2, client));
	client.clientResponse.setBool(true);
}

// This function sets directly the response with the parameter s.
// It will also be sent if the socket is ready for writing.
void Utils::setGoodResponse( const std::string &s, Client &client )
{
	client.clientResponse.setResponse(s);
	client.clientResponse.setBool(true);
}

// Deletes an allocated double pointer using delete.
void Utils::deleteDoublePtr( char **toDelete )
{
	for (int i = 0; toDelete[i]; i++)
		delete toDelete[i];
	delete toDelete;
}

// After parsing the request line, this function will be called.
// It will check for the validity of it.
void Utils::checkRequestLine( Client &client )
{
	if (!client.parsedRequest.hasAllowedChars())
		Utils::setErrorResponse(400, "HTTP/1.1 400 Bad Request\r\n", "Bad Request", client);
	else if (!client.parsedRequest.hasGoodSize())
		Utils::setErrorResponse(414, "HTTP/1.1 414 Request-URI too long\r\n", "Uri Too Long", client);
	else if (!client.parsedRequest.isSupported())
		Utils::setErrorResponse(405, "HTTP/1.1 405 Method Not Allowed\r\n", "Method Not Allowed", client);
	else if (!client.parsedRequest.isGoodVersion())
		Utils::setErrorResponse(505, "HTTP/1.1 505 Version Not Supported\r\n", "Version Not Supported", client);
}

// This function is used in the beggining of the main.
// Checks if arguments are valid and if the file given exists and has permissions.
void Utils::checkArgs( int ac, char **av )
{
	std::ifstream infile;
	std::string file;

	file = DEFAULT;
	if (ac > 2)
		throw "Invalid Arguments";
	if (av[1])
		file = av[1];
	infile.open(file);
	if (!infile.is_open())
		throw NotFoundError("Does Not Exist");
	else
		infile.close();
}

// After putting headers in a map, this function is called.
// It will check if headers are valid.
void Utils::checkHeaders( Client &client )
{
	std::istringstream s(client.parsedRequest.getValueFromMap("Content-Length"));
	std::string transferEnc;
	std::string contentType;

	// This part checks if there is an error in the request headers
	s >> client.contentLength;
	transferEnc = client.parsedRequest.getValueFromMap("Transfer-Encoding");
	contentType = client.parsedRequest.getValueFromMap("Content-Type");
	if (client.contentLength > client.correspondingBlock->get_body_size() * 1000000)
		Utils::setErrorResponse(413, "HTTP/1.1 413 Request Entity Too Large\r\n", "Request Entity Too Large", client);
	else if ((!transferEnc.empty() && transferEnc != "chunked")
		|| (transferEnc == "chunked" && contentType.find("multipart") != std::string::npos))
		Utils::setErrorResponse(501, "HTTP/1.1 501 Not Implemented\r\n", "Not Implemented", client);
	else if (client.parsedRequest.getMethod() == "POST" && client.contentLength == 0 && transferEnc.empty())
		Utils::setErrorResponse(400, "HTTP/1.1 400 Bad Request\r\n", "Bad Request", client);
	if (client.clientResponse.getBool() || client.parsedRequest.getMethod() != "POST")
		return ;

	// This part sets the correct reading mode for the body (in POST requests)
	Utils::setType(transferEnc, contentType, client);
}

// Sets the correct reading mode for the body in POST requests :
// Chunked, multipart, normal.
void Utils::setType( const std::string &transferEnc, const std::string &contentType, Client &client )
{
	if (!Utils::isLocationFormedWell(transferEnc, client))
		return ;
	if (transferEnc == "chunked")
	{
		client.shouldReadBody = true;
		client.bodyType = CHUNKED;
	}
	else if (contentType.find("multipart/form-data;") != std::string::npos)
	{
		int index = contentType.find("boundary=") + 9;
		client.boundary = "--" + contentType.substr(index, contentType.size() - index);
		client.shouldReadBody = true;
		client.bodyType = MULTIPART;
	}
	else if (client.contentLength > 0)
	{
		client.shouldReadBody = true;
		client.bodyType = OTHER;
	}
}

// If server_name instruction is in the config file
// this function will be called and it will check if the host header
// matches with the server name, if not then the client gets an error response.
bool Utils::serverNameMatches(const std::string &host, Serverblock *block)
{
	std::list<std::string>::iterator it;

	it = block->_serverNames.begin();
	if(host.empty() || (block->_serverNames.size() == 1 && *block->_serverNames.begin() == "_"))
		return(true);
	for(; it != block->_serverNames.end(); ++it)
		if(host == *it)
			return true;
	return (false);
}

// This function checks if the method in the request line is accepted in the server block.
bool Utils::isAccepted( const std::string &method, std::list< std::string > list )
{
	std::list< std::string >::iterator it1;

	it1 = list.begin();
	for (; it1 != list.end(); it1++)
		if (method == *it1)
			return true;
	return false;
}

// Particularly for POST requests, this function checks if everything is right before
// uploading files.
bool Utils::isLocationFormedWell( const std::string &transferEnc, Client &client )
{
	std::list< Location >::iterator currentList;
	
	if (client.correspondingBlock->_serverNames.size() > 0 && !Utils::serverNameMatches(client.parsedRequest.getValueFromMap("Host"), client.correspondingBlock))
	{
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "File Not Found", client);
		return false;
	}
	currentList = client.correspondingBlock->ifUriMatchLocationBlock(client.correspondingBlock->_location, client.parsedRequest.getUri());
	if (currentList == client.correspondingBlock->_location.end()
		|| (currentList->_supportUpload && (!currentList->checkIfPathExist(currentList->_upload_dir) || !currentList->ifRequestUriIsFolder(currentList->_upload_dir))))
	{
		Utils::setErrorResponse(404, "HTTP/1.1 404 Not Found\r\n", "Not Found", client);
		return false;
	}
	else if (!Utils::isAccepted("POST", currentList->_accept_list))
	{
		Utils::setErrorResponse(405, "HTTP/1.1 405 Not Allowed\r\n", "Method Not Allowed", client);
		return false;
	}
	else if (currentList->get_isThereRedirection())
	{
		Utils::setGoodResponse("HTTP/1.1 301 Moved Permanently\r\nLocation: " + currentList->_redirection[1] + "\r\n" + "Content-Length: 0\r\n" + Utils::getDateAndTime(), client);
		return false;
	}
	else if ((currentList->_currentRoot.back() == '/' && !currentList->get_indexes_location().size()
		&& !currentList->_supportUpload) || (!currentList->get_cgi() && !currentList->_supportUpload))
	{
		Utils::setErrorResponse(403, "HTTP/1.1 403 Forbidden\r\n", "Forbidden", client);
		return false;
	}
	else if (currentList->get_cgi() && !currentList->_supportUpload && client.contentLength > 0)
	{
		client.isThereCgi = true;
		client.shouldReadBody = true;
		client.bodyType = OTHER;
		if (transferEnc == "chunked")
			client.bodyType = CHUNKED;
		client.filePath = "/tmp/";
		return false;
	}
	client.filePath = currentList->_upload_dir;
	return true;
}

// The program takes no argument or 1 which is the file. (The config file)
// This function just returns the default path or the name of the file.
std::string Utils::getFileN( char **av )
{
	std::string file;

	file = DEFAULT;
	if (av[1])
		file = av[1];
	return file;
}

// This function returns the size of a string in the string format.
std::string Utils::getSizeInString( const std::string &str )
{
	std::stringstream ss;

	ss << str.size();
	return ss.str();
}

// This function is called when opening files.
// If the body in the requests has no filename, then a random name will be
// assigned to it with the appropriate extension.
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

// Get the size of file in a string. (used for content-length header in responses)
std::string	Utils::getSizeOfFile( const std::string &file )
{
	std::stringstream size;
	struct stat fileInfo;

	stat(file.c_str(), &fileInfo);
	size << fileInfo.st_size;
	return (size.str());
}

// This functions gets the current working directory.
std::string Utils::getPathInfo( void )
{
	char currentPath[FILENAME_MAX];

	if(getcwd(currentPath, sizeof(currentPath)) != NULL)
		return (std::string(currentPath));
	return (NULL);
}

// This functions gets all file content in a string.
std::string Utils::getFileContent( std::ifstream &file )
{
	std::stringstream buffer;

	buffer << file.rdbuf();
	return (buffer.str());
}

// Autoindex handler.
std::string	Utils::handleAutoindexFolder(const std::string &uri)
{
	DIR *dir;
	struct dirent *folder;
	std::stringstream response;

	response << "<html><body><ul>" <<std::endl;
	if((dir = opendir(uri.c_str())) != NULL)
	{
		while((folder = readdir(dir)) != NULL)
			if(folder->d_name[0] != '.')
				response << "<li><a href=\"" << folder->d_name << "\">" << folder->d_name << "</a></li>\n";
		closedir(dir);
	}
	response <<"</ul></body></html>"<<std::endl;
	return (response.str());	
}

// This function forms the whole error response with the correct status code
std::string Utils::formError( int statusCode, const std::string &statusLine, const std::string &msgInBody, Client &client )
{
	std::map< int, std::string >::iterator b;
	std::string returnString, fileInString;
	std::ifstream errorFile;
	std::stringstream s;

	b = client.correspondingBlock->_error_page.find(statusCode);
	if (b != client.correspondingBlock->_error_page.end())
		errorFile.open(b->second);
	if (b != client.correspondingBlock->_error_page.end() && !errorFile.is_open())
	{
		returnString = "HTTP/1.1 500 Internal Server Error\r\n";
		client.errString.setErrorFile(500, "Internal Server Error");
		s << client.errString.getFileInString().size();
		returnString += "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n" + Utils::getDateAndTime();
		return (returnString + client.errString.getFileInString());
	}
	else if (b != client.correspondingBlock->_error_page.end() && errorFile.is_open())
	{
		fileInString = std::string((std::istreambuf_iterator<char>(errorFile)), (std::istreambuf_iterator<char>()));
		s << fileInString.size();
		returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str() + "\r\n" + Utils::getDateAndTime();
		errorFile.close();
		return returnString + fileInString;
	}
	client.errString.setErrorFile(statusCode, msgInBody);
	s << client.errString.getFileInString().size();
	returnString = statusLine + "Content-Type: text/html\r\nContent-Length: " + s.str();
	returnString += "\r\n" + Utils::getDateAndTime();
	return (returnString + client.errString.getFileInString());
}

// This function transforms a hexa number to a decimal number.
// This one is used in chunked requests.
size_t Utils::giveDecimal( const std::string &hexaString )
{
	std::stringstream ss;
	size_t ret;

	ss << std::hex << hexaString;
	ss >> ret;
	return ret;
}

// This functions gets the size of a file.
size_t Utils::getSize( const std::string &file )
{
	struct stat fileInfo;

	stat(file.c_str(), &fileInfo);
	return (fileInfo.st_size);
}

// This function is used when creating environment variables for the cgi.
// It allocates using new instead of delete.
char *Utils::giveAllocatedChar( const std::string &str )
{
	char *returnString;

	returnString = new char[str.size() + 1];
	std::strcpy(returnString, str.c_str());
	returnString[str.size()] = '\0';
	return returnString;
}

// This function get the index in the appropriate location block.
std::string Utils::getIndex( Client &client )
{
	std::list<std::string>::iterator index = client.currentList->_indexes_location.begin();
	std::string joinPath;

	for(; index != client.currentList->_indexes_location.end(); ++index)
	{
		joinPath = client.currentList->_currentRoot + (*index);
		client.clientResponse.file.open(joinPath, std::ios::binary);
		if(client.clientResponse.file.is_open())
		{
			client.clientResponse.file.close();
			return joinPath;
		}
	}
	return client.currentList->_currentRoot;
}

// This function returns the date and time for the date header.
std::string Utils::getDateAndTime( void )
{
    char buff[40];
    std::time_t time;
    std::tm *tm;

    time = std::time(NULL);
    tm = std::gmtime(&time);
    std::strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S GMT", tm);
    return "Date: " + std::string(buff) + "\r\n\r\n";
}