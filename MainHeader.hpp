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


// Macros
#define MIN_TO_READ 1024
#define MAX_RQ 8192

// Classes header files
#include "Request.class.hpp"
#include "Client.class.hpp"
#include "Webserver.class.hpp"

// Structs used
struct Blocks
{
	int port;
	int ip;
	std::vector < std::string > index;
	std::string root;
};

// struct clients
// {
// 	char *response;
// 	char request[MAX];
// 	int bytesRead;
// 	int fd;
// 	struct sockaddr_in clientStruct;
// 	Request parsedRequest;
// };