#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
# include <sstream>
# include <list>

#include "Server.hpp"

class Server;
class Configfile
{
    private :
        std::list<std::string>       list;
    public :
        std::vector<Server>          server;       
        Configfile();
        std::string readfile(std::ifstream &infle);
        void        parse_file(std::string &config);
        void        parse_body(std::string &configfile);
        void        parse_line(std::string &line);
        int         check_names(std::string current_name, std::string value);

        void        trim_spaces_around(std::string &configfile);
        int         get_size_of_server() const;
        

        std::string         get_port();
        std::string         get_path();
        std::string         get_indexes();
        std::string         get_accept_list();

};
#endif