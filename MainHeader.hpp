#pragma once

// Main header files
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <iterator>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <poll.h>
#include <vector>
#include <list>
#include <map>

// The first macro is used when using recv function
// The second one is for the size of request line + headers
#define MIN_TO_READ 1024
#define MAX_RQ 8192

// Classes header files
#include "Response.class.hpp"
#include "Request.class.hpp"
#include "Client.class.hpp"
#include "Webserver.class.hpp"