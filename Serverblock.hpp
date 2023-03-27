#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <iostream>
#include <arpa/inet.h>
#include <list>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <vector>
#include <netinet/in.h>
#include <map>
#include "Exception.hpp"
#include "location.hpp"

// Should also consider checking the default path for a config file if there is no path
// Body_size max 2500mb; 

class Serverblock
{
private:
    int                     _port;
    in_addr_t               _ip;
    size_t                   _body_size;
    int                     _countlisten;
    int                     _countbodysize;
    int                     _count_location;
    bool                    _found;
public:
    std::list<std::string>   _serverNames;
    std::list<Location>     _location;
    Serverblock();
    Serverblock(std::string &block);
    Serverblock(const Serverblock &);
    Serverblock &operator=(const Serverblock &);
//***********************************Set Attributes functions****************************************//
    void                        set_port_and_ip(const std::string &line);
    void                        set_body_size(const std::string   &line);
    void                        set_error_page(const std::string &line);
    void                        set_server_name(const std::string &line);
//**********************************Functions_to_check_values*******************************//
    void                        block_is_empty(const std::string &block);
    void                        check_valid_value(const std::string &buffer,  std::string &value);
    void                        check_value_arg(const std::string &value);
//**********************************Function_to_check_valid_ip_and_port*******************************//
    bool                       check_valid_port(std::string &port);
    bool                       check_valid_ip(const std::string    &ip);
    bool                       check_valid_listen(const std::string &value, std::string &ip, std::string &port);
    bool                       is_Number(const std::string &buffer);
    bool                       chackValidIpValue(std::string &ip);
//**********************************Get Attributes functions*******************************//
    std::map<int, std::string> _error_page;
    struct sockaddr_in         socketNeeds;
    int                        get_port() const;
    in_addr_t                  get_ip() const;
    size_t                     get_body_size() const;
    std::list<Location>        get_locationblocks() const;
    std::list<std::string>     get_server_name();
    void                       check_valid_config();
    void                       check_duplicate();
    bool                       isNumberIp(const std::string &str);
    void                       check_valid_status_code(const std::string &key);
    std::list<Location>::iterator	ifUriMatchLocationBlock(std::list<Location> &list, const std::string &uri);
    ~Serverblock();
};


#endif