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
  * 'Make' is optional. (if you want to use the Makefile)
  * This project was developed under a unix environment (macOS). Compiling this code without errors in any other environment is not guaranteed.

## Usage
  1. In your terminal, clone this repo and go to the root directory : 
  ```bash
  git clone https://github.com/oessayeg/Webserver.git && cd Webserver
  ```
  2. Compilation and execution part : (If you have 'make' the first two commands will execute, otherwise the other two will)
  ```bash
  (make && ./webserv) || (c++ srcs/*.cpp && ./a.out)
  ```
  - :warning: The program needs a configuration file to be executed, so if you execute the program without any argument, the default path of the configuration file will be './default.conf', otherwise you can execute the program with the path of your own configuration file.
  ```bash
  (make && ./webserv /path/to/your/config/file) || (c++ srcs/*.cpp && ./a.out /path/to/your/config/file)
  ```
  
## Testing
Open your browser, in the address bar at the top of the browser window, type in :
  * **'localhost:7000'** to test GET requests (Note that the page that will appear is just a template took from w3schools)
  * **'localhost:7070'** to test php-cgi and cookies. After submitting the form, an image will be uploaded and a page will appear with that image. Everytime you open it up, the same page will appear because of cookies until you click on logout.
  * **'localhost:8000'** to test python cgi. A simple form should be filled, and a simple python script will be executed on it.
  * **'localhost:8080'** to test file uploads (Files will be uploaded on a folder 'uploads'). If you want to get them back go to 'localhost:8080/uploads/nameOfYourFile'

## Configuration file directives
  * 'server' : the configuration block that defines the settings for serving content on a specific port. (The directives below can be only inside server blocks)
  * listen 'port' : sets this port to listen to incoming connections.
  * error_page 'statusCode' 'pathToErrorFile' : if an error with the status code 'statusCode' occurs, the page of your choice will appear instead of the default error page.
  * body_size 'size in megabytes' : sets the maximum size of file uploads.
  * location '/path/to/your/directory' : it defines how the server will handle requests for a specific url. (The directives below can be only inside location blocks)
    * accept_list 'method1' 'method2' : sets the allowed http methods. If not specified, GET, POST and DELETE are the default ones.
    * run_cgi 'on/off' : sets cgi execution (off by default)
    * root '/path/to/directory' : specifies the root directory that will be used to serve files for a given location.
    * index 'file1' 'file2' : specifies the files that will be served if the request uri is a folder.
    * upload_dir 'directory' : specifies the directory where the files will be uploaded.

## Example of a simple configuration file
<img width="513" alt="Capture d’écran 2023-04-14 à 14 30 47" src="https://user-images.githubusercontent.com/96997041/232073486-9d45abf8-3a8b-4a1d-9973-73405d51e4ef.png">

## Project status
  * Finished
