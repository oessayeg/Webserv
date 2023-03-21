#pragma once

// Main header files
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
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
#include <limits.h>
#include <dirent.h>
#include <map>

// The first macro is used when using recv function
// The second one is for the size of request line + headers
#define MIN_TO_READ 8192
#define BUFF_SIZE 17825
#define MAX_RQ 8192
#define ROOTFOLDER -42
// Body type macros

#define CHUNKED_MULTIPART 1
#define MULTIPART 2
#define CHUNKED 3
#define OTHER 4

//Should consider changing the int to short (if I don't need the whole bytes)
// Classes header files
#include "MimeTypes.class.hpp"
#include "Response.class.hpp"
#include "Request.class.hpp"
#include "Serverblock.hpp"
#include "Client.class.hpp"
#include "BodyParser.class.hpp"
#include "Utils.class.hpp"