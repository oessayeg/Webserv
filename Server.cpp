#include "Server.hpp"


Server::Server()
{

}

Server::Server(std::string &block)
{
    std::stringstream outfile(block);
    std::string line;
    std::string location;
    while(getline(outfile, line))
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
            else if (line.substr(0, 8) == "location")
            {
                location = line + "\n";
                while(getline(outfile, line))
                {
                    location += line + "\n";
                    if (line.find("}") != std::string::npos)
                        break;
                }
                _location.push_back(Location(location));
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
        if (std::isdigit(str[i]) == 0 && str[i] != '.')
            return false;
    }
    return true;
}

void    Server::set_listen(std::string   listen)
{
    size_t found = listen.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = listen.find_first_of(";");
    if (found_t == std::string::npos)
            throw "'listen' value should be end with ';' ";
    std::string value = listen.substr(found, found_t - found);
    if (is_Number(value) && check_validIp(value))
        this->_listen = value;
    else
        throw "listen value should be number";
}

void    Server::set_root_path(std::string root_path)
{
    size_t  found = root_path.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = root_path.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'root_path' value should be end with ';' ";
    std::string name = root_path.substr(found, found_t - found);
    found = name.find_first_of(" \t\f\v\n\r");
    found_t = name.find_first_not_of(" \t\f\v\n\r", found);
    if (found_t != std::string::npos)
        throw "Invalid Path";
    this->_root_path = name;
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

void            Server::set_body_size(std::string body_size)//you shuld check if mega or gb ....
{
    size_t found = body_size.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = body_size.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'accept_list' value should be end with ';' ";
    std::string name = body_size.substr(found, found_t - found);
    this->_body_size = name;
}

bool        Server::check_validIp(std::string &name)
{
    int count;

    count = 0;
    size_t found = name.find_first_of(".");
    size_t  found_t = name.find_first_of(".");
    if (found == std::string::npos)
        return(true);
    while (found_t != std::string::npos)
    {
        count++;
        found_t = name.find_first_of(".", found_t + 1);
    }
    if(count != 3)
        throw "Invalid Ip";
    if (found != std::string::npos)
    {
        std::stringstream outfile(name);
        std::string line;
        while (getline(outfile, line, '.'))
            check_valid_numIp(line);
    }
    return (true);
}

void             Server::check_valid_numIp(std::string &value)
{
    if (value.length() > 3)
        throw "Invalid Ip";
    int num = atoi(value.c_str());
    if (num > 255)
        throw "Ip is not in valid Range";
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

Server::~Server()
{

}