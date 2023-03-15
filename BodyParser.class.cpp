#include "BodyParser.class.hpp"

BodyParser::BodyParser( void ) { }

BodyParser::BodyParser( const BodyParser &rhs ) { }

BodyParser &BodyParser::operator=( const BodyParser &rhs )
{
	return *this;
}

BodyParser::~BodyParser( void ) { }

void BodyParser::parseChunkedData( Client &client )
{
	size_t i;
	size_t index2;

	if (!client.gotFileName)
		this->_openWithProperExtension(client.parsedRequest._headers["Content-Type"], client);
	i = 0;
	if (client.bytesToRead == 0)
	{
		for (; client.request[i] != '\r'; i++);
		client.bytesToRead = _giveDecimal(std::string(client.request, client.request + i));
		i += 2;
		if (client.bytesToRead == 0)
		{
			client.finishedBody = true;
			client.fileToUpload.close();
			return ;
		}
	}
	index2 = i;
	for (; index2 < client.bytesToRead + i && index2 < client.bytesRead; index2++);
	client.fileToUpload.write(client.request + i, index2 - i);
	client.bytesToRead -= index2 - i;
	client.bytesCounter += index2 - i;
	if (client.contentLength > 0 && client.bytesCounter == client.contentLength)
	{
		client.finishedBody = true;
		client.fileToUpload.close();
	}
	else if (index2 == client.bytesRead)
	{
		memset(client.request, 0, MAX_RQ);
		client.bytesRead = 0;
	}
	else
	{
		memmove(client.request, &client.request[index2 + 2], (client.bytesRead - (index2 + 2)) + 1);
		client.bytesRead -= (index2 + 2);
		if (client.bytesRead == 0)
			return ;
		std::cout << client.bytesRead << std::endl;
		this->parseChunkedData(client);
	}
}

void BodyParser::parseNormalData( Client &client )
{
	int i;

	if (!client.gotFileName)
		this->_openWithProperExtension(client.parsedRequest._headers["Content-Type"], client);
	for (i = 0; i < client.bytesRead; i++);
	client.bytesCounter += i;
	client.fileToUpload.write(client.request, i);
	if (client.bytesCounter == client.contentLength)
	{
		client.fileToUpload.close();
		client.finishedBody = true;
		return ;
	}
	// Need to change MAX_RQ to bytes read
	memset(client.request, 0, MAX_RQ);
	client.bytesRead = 0;
}

void BodyParser::parseMultipartData( Client &client )
{
	bool isFound;
	int i;

	isFound = false;
	if (!this->_isThereFilename(MULTIPART, client))
		return ;
	for (i = 0; i < client.bytesRead; i++)
	{
		if (client.request[i] == '\r' && i + 2 < client.bytesRead && client.request[i + 2] == '-')
			isFound = this->_isBoundary(&client.request[i + 2], client);
		if (isFound)
			break;
	}
	if (!client.shouldSkip)
		client.fileToUpload.write(client.request, i);
	if (!isFound)
	{
		memset(client.request, 0, client.bytesRead);
		client.bytesRead = 0;
		return ;
	}
	i += 2;
	client.bytesRead -= i;
	client.shouldSkip = false;
	client.gotFileName = false;
	client.fileToUpload.close();
	if (client.request[i + client.boundary.size()] == '-' && client.request[i + client.boundary.size() + 1] == '-')
	{
		client.finishedBody = true;
		return ;
	}
	memmove(client.request, &client.request[i], client.bytesRead + 1);
	this->parseMultipartData(client);
}

bool BodyParser::_isThereFilename( int bodyType, Client &client )
{
	char *crlfIndex;
	char *fileIndex;
	int i;

	if (!client.gotFileName)
	{
		if (bodyType == MULTIPART)
			crlfIndex = strstr(client.request, "\r\n\r\n");
		else if (bodyType == CHUNKED_MULTIPART)
			crlfIndex = strstr(client.request, "\r\n\r\n\r\n");
		if (crlfIndex == NULL)
			return false;
		else
		{
			fileIndex = strstr(client.request, "filename=");
			if (fileIndex != NULL && *(fileIndex + 10) != '\"')
			{
				fileIndex += 10;
				client.gotFileName = true;
				this->_openFile(fileIndex, client);
			}
			else
				client.shouldSkip = true;
			if (bodyType == MULTIPART)
			{
				for (i = 0; client.request + i < crlfIndex + 4; i++);
				memmove(client.request, crlfIndex + 4, (client.bytesRead - i) + 1);
				client.bytesRead -= i;
			}
			else if (bodyType == CHUNKED_MULTIPART)
			{
				for (i = 0; client.request + i < crlfIndex + 6; i++);
				memmove(client.request, crlfIndex + 6, (client.bytesRead - i) + 1);
				client.bytesRead -= i;
			}
		}
	}
	return true;
}

bool BodyParser::_isBoundary( char *ptr, Client &client )
{
	int i;

	i = 0;
	while (i < client.bytesRead && ptr[i] == client.boundary[i])
		i++;
	return i == client.boundary.size();
}

size_t BodyParser::_giveDecimal( const std::string &hexaString )
{
	std::stringstream ss;
	size_t ret;

	ss << std::hex << hexaString;
	ss >> ret;
	return ret;
}

void BodyParser::_openFile( char *name, Client &client )
{
	char *fileName;
	int i;

	for (i = 0; name[i] != '\"'; i++);
	fileName = new char[i + 1];
	for (i = 0; name[i] != '\"'; i++)
		fileName[i] = name[i];
	fileName[i] = '\0';
	client.fileToUpload.open(fileName, std::ios::trunc | std::ios::binary);
	delete fileName;
	return ;
}

void BodyParser::_openWithProperExtension( const std::string &contentType, Client &client )
{
	std::string extension;

	extension = this->_extensions.getExtension(contentType);
	client.fileToUpload.open(this->_randomString() + extension, std::ios::trunc | std::ios::binary);
	client.gotFileName = true;
}

std::string BodyParser::_randomString( void )
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