#pragma once

// Main header files
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h> // To remove after finishing, just for memset
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <list>
#include <map>

// Classes header files
#include "Webserver.class.hpp"
#include "Request.class.hpp"

// Macros
#define MIN_TO_READ 1024
#define MAX_BODY_SIZE 10240 //102400000
#define MAX_REQUEST_HEADERS 3072 + 4096 // Request line + headers
#define MAX MAX_BODY_SIZE + MAX_REQUEST_HEADERS

// Structs used
struct blocks
{
	int port;
	int ip;
	std::vector < std::string > index;
	std::string root;
};

struct clients
{
	char *response;
	char request[MAX];
	int bytesRead;
	int fd;
	struct sockaddr_in clientStruct;
	Request parsedRequest;
};