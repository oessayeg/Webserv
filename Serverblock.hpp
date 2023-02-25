#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <iostream>
#include <list>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include "Exception.hpp"
#include "location.hpp"
class Serverblock
{
private:
    int                     _port;
    std::string              _ip;
    std::string             _hostname;
    long                    _body_size;
    std::string             _root;
    int                     _countlisten;
    int                     _countbodysize;
    int                     _countroot;
    std::vector<std::string>  _indexes;
    std::vector<Location>  _location;
public:
    Serverblock();
    Serverblock(std::string &block);

//***********************************Set Artibiout function****************************************//
    void        set_port_and_ip(std::string line);
    void        set_body_size(std::string   line);
    void        set_root(std::string         line);
    void        set_index(std::string         line);
//**********************************Function_to_check_values*******************************//
    void        block_is_empty(std::string &block);
    void        check_valid_value(std::string buffer,  std::string &value);
    void        check_value_arg(std::string value);
//**********************************Function_to_check_valid_ip_and_port*******************************//
    bool        check_valid_port(std::string &port);
    bool        check_valid_ip(std::string    &ip);
    bool        check_valid_listen(std::string buffer, std::string &ip, std::string &port);
    bool        is_Number(std::string buffer);
    void        init_ip_and_port(std::string &ip, std::string &port);
    void        check_valid_numIp(std::string &value);
//**********************************Get Artibiout function *******************************//
    int         get_port() const;
    std::string get_ip() const;
    long        get_body_size() const;
    std::string get_root() const;
    std::vector<Location> get_locationblocks() const;
};


#endif