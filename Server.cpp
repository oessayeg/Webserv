#include "Server.hpp"


Server::Server()
{

}

Server::Server(std::string &block)
{
    std::stringstream outfile(block);
    std::string line;

    while(getline(outfile, line)) //Should be add error if there valuse after ';' and skip comment
        {
            size_t found = line.find_first_not_of(" \t\f\v\n\r{");
            size_t found_t = line.find_last_not_of(" \t\f\v\n\r}");
            if (found == std::string::npos || found_t == std::string::npos)
                continue;
            line = line.substr(found, found_t - found + 1);
            if(line.substr(0, 6) == "listen")
                set_listen(line.substr(6));
            else if (line.substr(0, 5) == "index")
                set_indexes(line.substr(5));
            else if (line.substr(0, 11) == "server_name")
                set_server_name(line.substr(11));
            else if (line.substr(0, 4) == "root")
                set_root_path(line.substr(4));
            else if (line.substr(0, 11) == "accept_list")
                set_accept_list(line.substr(11));
            else if (line.substr(0, 8) == "location")
            {
            }
            else if(line.substr(0,9) == "body_size")
            {
                set_body_size(line.substr(9));
            }
            else
                 throw "There Invalid Name";
    }
}

bool Server::is_Number(std::string  str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if (std::isdigit(str[i]) == 0)
            return false;
    }
    return true;
}

void    Server::set_listen(std::string   listen)//Should Add Part Ip
{
    size_t found = listen.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = listen.find_first_of(";");
    if (found_t == std::string::npos)
            throw "'listen' value should be end with ';' ";
    std::string value = listen.substr(found, found_t - found);
    if (is_Number(value))
        this->_listen = value;
    else
        throw "listen value should be number";
}

void    Server::set_root_path(std::string root_path)
{
    size_t  found = root_path.find_first_not_of("   \t\f\v\n\r");
    size_t found_t = root_path.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'root_path' value should be end with ';' ";
    this->_root_path = root_path.substr(found, found_t - found);
}

void    Server::set_indexes(std::string indexes)
{
    size_t found = indexes.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = indexes.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'indexes' value should be end with ';' ";
    std::string name = indexes.substr(found, found_t - found);
    std::stringstream paths(name);
    std::string line;
    while(getline(paths, line, ' '))
        this->_indexes.push_back(line);
}

void    Server::set_server_name(std::string   server_name)
{
    size_t found = server_name.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = server_name.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'server_name' value should be end with ';' ";
    this->_server_name = server_name.substr(found, found_t - found);
}

void    Server::set_accept_list(std::string accept_list)
{
    size_t found = accept_list.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = accept_list.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'accept_list' value should be end with ';' ";
    std::string name = accept_list.substr(found, found_t - found);
    std::stringstream list(name);
    std::string line;
    while(getline(list, line, ' '))
        _accept_list.push_back(line);
}

void            Server::set_body_size(std::string body_size)//you shuld check if mega or gb ....
{
    size_t found = body_size.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = body_size.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'accept_list' value should be end with ';' ";
    std::string name = body_size.substr(found, found_t - found);
    this->_body_size = name;
}

std::string     Server::get_port() const
{
    return (this->_listen);
}

std::string     Server::get_path() const
{
    return (this->_root_path);
}

std::vector<std::string>     Server::get_indexes() const
{
    return (this->_indexes);
}

std::vector<std::string>     Server::get_accept_list() const
{
    return (this->_accept_list);
}

Server::~Server()
{

}