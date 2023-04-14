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
 * The 'default.conf' file provided here is set to test all features in different ports.
 * Here's how you can test them if default.conf is used (if you used your own config file the behavior will of course change) :
 * Open your browser and go to :
   * **localhost:7000'** to test GET requests (Note that the page that will appear is just a template took from w3school)
   * **'localhost:7070'** to test php-cgi and cookies. After filling the form, an image will be uploaded and a page will appear with that image. Everytime you open it up, the same page will appear because of cookies until you click on logout.
   * 'localhost:8000' to test python cgi. A simple form should be filled, and a simple python script will be executed on it.
   * 'localhost:8080' to test file uploads (Files will be uploaded on a folder 'uploads'). If you want to get them back go to 'localhost:8080/uploads/nameOfYourFile'
