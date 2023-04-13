# Webserv
An http server implemented using C++98.

## Overview
 * This is a single threaded web server that was developed using I/O mutliplexing to serve concurrent clients.
 * It is implemented in C++98 using the socket api.
 * It is designed to serve static web content for small sized projects.
 * The http methods supported are : GET, POST and DELETE.
 * It supports cgi (php and python) and cookies.
 
   <img width="979" alt="Capture d’écran 2023-04-13 à 15 48 22" src="https://user-images.githubusercontent.com/96997041/231814758-4865f24b-a42e-4b2f-ab75-5ee27b3ed004.png">

## Setup
  * Clang should be installed. (Obviously to compile C++ code)
  * Make is optional. (if you want to use the Makefile)
  * This project was developed under a unix environment (macOS). Compiling this code without errors in any other environment is not guaranteed.

## Usage
  1. In your terminal, clone this repo and go to the root directory : 
  ```bash
  git clone https://github.com/oessayeg/Webserver.git && cd Webserver
  ```
  2. Compilation part : (If you have make the first two commands will execute, otherwise the other two will)
  ```bash
  (make && ./webserv) || (c++ srcs/*.cpp && ./a.out)
  ```
  
