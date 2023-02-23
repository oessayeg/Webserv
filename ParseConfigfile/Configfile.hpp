#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
# include <sstream>
# include <list>

#include "../Exception.hpp"
#include "../Serverblock.hpp"

class Serverblock;
class Configfile
{
    private :
        std::list<std::string>            list;
        std::list<Serverblock>          server;       
    public :
        Configfile();
        void                check_errors(std::string &namefile);
        std::string         get_contentfile(std::ifstream &file);
        void                parse_configfile(std::string        &configfile);


        std::list<Serverblock>      get_serverblocks();
};
#endif