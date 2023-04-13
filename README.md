# Webserv
An http server implemented using C++98.

# Overview
 * This is a single threaded web server that was developed using I/O mutliplexing to serve concurrent clients.
 * It is developed in C++98 using the socket api.
 * It is designed to serve static web content for small sized projects.
 * The methods supported on this web server are : GET, POST and DELETE.
 * It supports cgi (php and python).
 
## Setup
  * Clang should be installed. (Obviously to compile C++ code)
  * Make is optional. (if you want to use the Makefile)
  * This project was developed under a unix environment (macOS). Compiling this code without errors in any other environment is not guaranteed.

