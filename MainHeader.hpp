#pragma once

// Main header files
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h> // To remove after finishing, just for memset
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <list>
#include <map>

// Server header files
#include "Webserver.class.hpp"

// Macros
#define MAX_TO_READ 1000
#define MAX_TO_SEND 10000

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
	char response[MAX_TO_READ];
	char request[MAX_TO_SEND];
	int bytesRead;
	int fd;
};