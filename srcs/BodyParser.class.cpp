#include "../includes/BodyParser.class.hpp"

BodyParser::BodyParser( void ) { }

BodyParser::BodyParser( const BodyParser &rhs ) { *this = rhs; }

BodyParser &BodyParser::operator=( const BodyParser &rhs )
{
	this->_extensions= rhs._extensions;
	return *this;
}

BodyParser::~BodyParser( void ) { }

// This function checks the bodyType and calls the appropriate
// body parsing function.
void BodyParser::chooseCorrectParsingMode( Client &client )
{
	if (client.bodyType == CHUNKED)
		this->parseChunkedData(client);
	else if (client.bodyType == MULTIPART)
		this->parseMultipartData(client);
	else if (client.bodyType == OTHER)
		this->parseNormalData(client);
}

// Multipart-form-data parsing function.
void BodyParser::parseMultipartData( Client &client )
{
	size_t i;
	bool isFound;

	isFound = false;
	if (!this->_isThereFilename(client))
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
		client.fileToUpload.write("\0", 1);
		client.finishedBody = true;
		return ;
	}
	memmove(client.request, &client.request[i], client.bytesRead + 1);
	this->parseMultipartData(client);
}

// Chunked request function.
void BodyParser::parseChunkedData( Client &client )
{
	size_t i;
	size_t index2;

	i = 0;
	// If the file isn't opened yet, generate a random name for it with the proper extension.
	if (!client.gotFileName)
		this->_openWithProperExtension(client.parsedRequest.getValueFromMap("Content-Type"), client);

	// Get the hexadecimal and skip it.
	if (client.bytesToRead == 0)
	{
		if (!_isHexaReadable(client))
			return ;
		for (; client.request[i] != '\r' && i < client.bytesRead; i++);
		if (i == client.bytesRead || client.request[i + 1] != '\n')
			return ;
		client.bytesToRead = Utils::giveDecimal(std::string(client.request, client.request + i));
		i += 2;
		if (client.bytesToRead == 0)
		{
			client.finishedBody = true;
			client.fileToUpload.close();
			return ;
		}
	}

	// This is the part where the actual reading starts after skipping the hexa.
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
		memset(client.request, 0, MIN_TO_READ);
		client.bytesRead = 0;
	}
	else
	{
		this->_moveRequest(index2, client);
		if (client.bytesRead == 0)
			return ;
		this->parseChunkedData(client);
	}
}

// This function keeps reading data until the content-length is reached.
void BodyParser::parseNormalData( Client &client )
{
	size_t i;

	if (!client.gotFileName)
		this->_openWithProperExtension(client.parsedRequest.getValueFromMap("Content-Type"), client);
	for (i = 0; i < client.bytesRead; i++);
	client.bytesCounter += i;
	client.fileToUpload.write(client.request, i);
	if (client.bytesCounter == client.contentLength)
	{
		client.fileToUpload.close();
		client.finishedBody = true;
		return ;
	}
	memset(client.request, 0, MIN_TO_READ);
	client.bytesRead = 0;
}

std::string BodyParser::getContentType( const std::string &file )
{
	return this->_extensions.getContentType(file);
}

// These 2 next functions open files with their proper extension and path.
// For POST requests with cgi, the file will be put in /tmp and removed
// immediatly after executing the cgi.
void BodyParser::_openWithProperExtension( const std::string &contentType, Client &client )
{
	std::string extension;

	extension = this->_extensions.getExtension(contentType);
	client.nameForCgi = client.filePath + "/" + Utils::generateRandomString() + extension;
	client.fileToUpload.open(client.nameForCgi, std::ios::trunc | std::ios::binary);
	client.gotFileName = true;
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
	client.nameForCgi = client.filePath + "/" + fileName;
	client.fileToUpload.open(client.nameForCgi, std::ios::trunc | std::ios::binary);
	delete fileName;
	return ;
}

void BodyParser::_moveRequest( size_t index2, Client &client )
{
	int i;

	i = 0;
	if (index2 + 1 == client.bytesRead)
		i = 1;
	else
		i = 2;
	memmove(client.request, &client.request[index2 + i], (client.bytesRead - (index2 + i)) + 1);
	client.bytesRead -= (index2 + i);
}

// This function is used in multipart-form-data reading.
// It gets the filename and opens a file where the data will be sent.
bool BodyParser::_isThereFilename( Client &client )
{
	char *crlfIndex;
	char *fileIndex;
	int i;

	crlfIndex = NULL;
	fileIndex = NULL;
	if (!client.gotFileName)
	{
		crlfIndex = strstr(client.request, "\r\n\r\n");
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
			for (i = 0; client.request + i < crlfIndex + 4; i++);
			memmove(client.request, crlfIndex + 4, (client.bytesRead - i) + 1);
			client.bytesRead -= i;
		}
	}
	return true;
}

// This function indicates if the boundary is reached or not.
bool BodyParser::_isBoundary( char *ptr, Client &client )
{
	size_t i;

	i = 0;
	while (i < client.bytesRead && ptr[i] == client.boundary[i])
		i++;
	return (i == client.boundary.size());
}

// This function checks if the body in chunked requests contains all the hexa or not.
bool BodyParser::_isHexaReadable( Client &client )
{
	size_t i;

	for (i = 0; client.request[i] != '\r' && i < client.bytesRead; i++);
	if (i == client.bytesRead)
		return false;
	if (i == 0 && client.request[i] == '\r' && client.request[i + 1] == '\n')
	{
		memmove(client.request, client.request + 2, client.bytesRead - 1);
		client.bytesRead -= 2;
	}
	if (client.bytesRead == 0)
		return false;
	return true;
}
