#pragma once

#include "MainHeader.hpp"
#include "ErrorString.class.hpp"

// This class contains just attributes that will be useful for
// getting the client's request/body and therefore preparing an
// appropriate response for him.
class Client
{
	// This is the socket from where the request is read and the response is sent.
	private :
		int _socket;

	public :
		struct sockaddr_in *clientStruct;
		char request[BUFF_SIZE + 1];
		int bodyType;
		bool isRead;
		bool isRqLineParsed;
		bool isHeaderParsed;
		bool shouldReadBody;
		bool finishedBody;
		bool gotFileName;
		bool shouldSkip;
		bool isConnected;
		bool isThereCgi;
		size_t bytesRead;
		size_t bytesToRead;
		size_t bytesCounter;
		size_t contentLength;
		std::ofstream fileToUpload;
		std::string stringRequest;
		std::string boundary;
		std::string filePath;
		std::string nameForCgi;
		std::list< Location >::iterator currentList;

	public :
		Serverblock *correspondingBlock;
		Response clientResponse;
		ErrorString errString;
		Request parsedRequest;

	// Coplien's form.
	public :
		Client( void );
		Client( const Client &rhs );
		Client &operator=( const Client &rhs );
		~Client( void );

	// Accessors for the _socket variable.
	public :
		void setSocket( int s );
		int getSocket( void ) const;
};