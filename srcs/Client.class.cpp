#include "../includes/MainHeader.hpp"

Client::Client( void ) : _socket(0),\
		clientStruct(new struct sockaddr_in), bodyType(0), isRead(false), \
		isRqLineParsed(false), isHeaderParsed(false), shouldReadBody(false), \
		finishedBody(false), gotFileName(false), shouldSkip(false), isConnected(true), \
		isThereCgi(false), bytesRead(0), bytesToRead(0), bytesCounter(0), \
		contentLength(0), correspondingBlock(NULL), errString(), parsedRequest() { }

Client::Client( const Client &rhs )
{
	*this = rhs;
}

Client &Client::operator=( const Client &rhs )
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->clientStruct = new struct sockaddr_in;
		if (this->clientStruct != NULL)
			*this->clientStruct = *rhs.clientStruct;
		*this->request = *rhs.request;
		this->bodyType = rhs.bodyType;
		this->isRead = rhs.isRead;
		this->isRqLineParsed = rhs.isRqLineParsed;
		this->isHeaderParsed = rhs.isHeaderParsed;
		this->shouldReadBody = rhs.shouldReadBody;
		this->finishedBody = rhs.finishedBody;
		this->gotFileName = rhs.gotFileName;
		this->shouldSkip = rhs.shouldSkip;
        this->isConnected = rhs.isConnected;
		this->isThereCgi = rhs.isThereCgi;
		this->bytesRead = rhs.bytesRead;
		this->bytesToRead = rhs.bytesToRead;
		this->bytesCounter = rhs.bytesCounter;
		this->contentLength = rhs.contentLength;
		this->stringRequest = rhs.stringRequest;
		this->boundary = rhs.boundary;
		this->filePath = rhs.filePath;
		this->nameForCgi = rhs.nameForCgi;
		this->currentList = rhs.currentList;
		this->correspondingBlock = new Serverblock;
		if (rhs.correspondingBlock != NULL)
			*this->correspondingBlock = *rhs.correspondingBlock;
		this->clientResponse = rhs.clientResponse;
		this->errString = rhs.errString;
		this->parsedRequest = rhs.parsedRequest;
	}
	return *this;
}

Client::~Client( void )
{
	delete this->clientStruct;
	if (correspondingBlock)
		delete correspondingBlock;
}

void Client::setSocket( int s )
{
	this->_socket = s;
}

int Client::getSocket( void ) const
{
	return (this->_socket);
}