#pragma once

// Main header files.
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <iostream>
#include <string.h>
#include <iterator>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <poll.h>
#include <vector>
#include <list>
#include <limits.h>
#include <dirent.h>
#include <map>

// Default path for the config file.
#define DEFAULT   "./default.conf"

// MIN_TO_READ = how much bytes to read when using recv().
#define MIN_TO_READ 8192

// BUFF_SIZE = the size of the buffer that'll contain the request and body of the client.
#define BUFF_SIZE 17825

// Body type macros (To know how to parse bodies in POST requests).
#define CHUNKED_MULTIPART 1
#define MULTIPART 2
#define CHUNKED 3
#define OTHER 4

#include "MimeTypes.class.hpp"
#include "Response.class.hpp"
#include "Request.class.hpp"
#include "Serverblock.hpp"
#include "Client.class.hpp"
#include "BodyParser.class.hpp"
#include "Utils.class.hpp"