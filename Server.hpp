#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "location.hpp"

class   Location;
class Server
{
    private : 
        std::string                 _listen;
        std::string                 _root_path;
        std::string                 _server_name;
        std::string                 _body_size;
        std::vector<std::string>    _indexes;
        std::vector<Location>       _location;
    public:

        Server();
        Server(std::string &block);
    /****************************************Set   Attribute***************************************************/
        void                        set_listen(std::string   listen);
        void                        set_root_path(std::string root_path);
        void                        set_server_name(std::string   server_name);
        void                        set_indexes(std::string indexes);
        void                        set_body_size(std::string body_size);

    /****************************************Get   Attribute***************************************************/
        std::string                 get_port() const;
        std::string                 get_path() const;
        std::vector<std::string>    get_indexes() const;
    /****************************************Function***************************************************/
        bool                        is_Number(std::string str);
        bool                        check_validIp(std::string &name);
        void                        check_valid_numIp(std::string &value);

        ~Server();
};

#endif