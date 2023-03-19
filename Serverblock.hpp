#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <iostream>
#include <list>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <map>
#include "Exception.hpp"
#include "location.hpp"

// Should also consider checking the default path for a config file if there is no path
// Do not forget the comments in the config file
// Body_size max 2500mb; 

class Serverblock
{
private:
    int                     _port;
    std::string              _ip;
    size_t                   _body_size;
    int                     _countlisten;
    int                     _countbodysize;
    int                     _count_location;
    bool                    _found;
public:
    std::list<Location>     _location;
    Serverblock();
    Serverblock(std::string &block);
    Serverblock(const Serverblock &);
    Serverblock &operator=(const Serverblock &);
//***********************************Set Attributes functions****************************************//
    void                        set_port_and_ip(std::string line);
    void                        set_body_size(std::string   line);
    void                        set_error_page(std::string line);
//**********************************Functions_to_check_values*******************************//
    void                        block_is_empty(std::string &block);
    void                        check_valid_value(std::string buffer,  std::string &value);
    void                        check_value_arg(std::string value);
//**********************************Function_to_check_valid_ip_and_port*******************************//
    bool                       check_valid_port(std::string &port);
    bool                       check_valid_ip(std::string    &ip);
    bool                       check_valid_listen(std::string buffer, std::string &ip, std::string &port);
    bool                       is_Number(std::string buffer);
    void                       init_ip_and_port(std::string &ip, std::string &port);
    void                       check_valid_numIp(std::string &value);
//**********************************Get Attributes functions*******************************//
    std::map<int, std::string> _error_page;
    struct sockaddr_in         socketNeeds;
    int                        get_port() const;
    std::string                get_ip() const;
    size_t                       get_body_size() const;
    std::list<Location>        get_locationblocks() const;
    void                       check_valid_config();
    void                       check_duplicate();
    void                       check_valid_status_code(std::string key);
    std::list<Location>::iterator	ifUriMatchLocationBlock(std::list<Location> &list, const std::string &uri);
    ~Serverblock();
};


#endif