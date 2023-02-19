#include "location.hpp"

Location::Location():_auto_index(false)
{
}

Location::Location(std::string &location)
{
    size_t find = location.find_first_of("/");
    if (find == std::string::npos)
        throw "location Path not valid";
    std::stringstream outfile(location);
    std::string line;
    while (getline(outfile, line))
    {
        size_t found_t = line.find_first_not_of(" \t\f\v\n\r");
        size_t found_tt = line.find_last_not_of(" \t\f\v\n\r");
        if (found_t == std::string::npos)
                continue;
        line = line.substr(found_t, found_tt - found_t + 1);
        if (line.substr(0, 8) == "location")
            set_path_location(line.substr(8));
        else if (line.substr(0, 4) == "root")
           set_root_location(line.substr(4));
        else if (line.substr(0, 9) == "autoindex")
            set_autoindex_location(line.substr(10));
        else if (line.substr(0, 11) == "accept_list")
            set_accept_list_location(line.substr(11));
        else if(line.substr(0, 5) == "index")
            set_indexes_location(line.substr(5));
    } 
}

void        Location::set_path_location(std::string path_location)
{
    size_t found = path_location.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = path_location.find_first_of("/");
    if (found_t == std::string::npos)
        throw "invalid arg in location";
    found = path_location.find_first_not_of(" \t\f\v\n\r", found_t + 1);
    if (found != std::string::npos)
    {
        std::string name = path_location.substr(found, path_location.find_last_not_of(" \t\f\v\n\r") - found);
        std::stringstream oufile(name);
        std::string       line;
        while (getline(oufile, line, ' '))
            _path_location.push_back(line);
    }
    else
        _path_location.push_back("/");
}
 
void        Location::set_root_location(std::string root_location)
{
    size_t found = root_location.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = root_location.find_first_of(";");
    if (found_t == std::string::npos)
            throw "value should be end with ';' ";
    std::string name = root_location.substr(found, found_t - found);
    found = name.find_first_of(" \t\f\v\n\r");
    found_t = name.find_first_not_of(" \t\f\v\n\r", found);
    if (found_t != std::string::npos)
        throw "invalid root";
    this->_root = name;
}

void    Location::set_accept_list_location(std::string accept_list)
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

void        Location::set_autoindex_location(std::string index_location)
{
    size_t found = index_location.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = index_location.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'accept_list' value should be end with ';' ";
    std::string name = index_location.substr(found, found_t - found);
    found = name.find_first_of(" \t\f\v\n\r");
    found_t = name.find_first_not_of(" \t\f\v\n\r", found);
    if (found_t != std::string::npos)
        throw "invalid auto index";
    if (name.substr(0, 2) == "on")
        this->_auto_index = true;
    else if (name.substr(0, 3) == "off")
        this->_auto_index = false;
    else
        throw "invalid arg";
}

void        Location::set_indexes_location(std::string        indexes_location)
{
    size_t found = indexes_location.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = indexes_location.find_first_of(";");
    if (found_t == std::string::npos)
        throw "'indexes' value should be end with ';' ";
    std::string name = indexes_location.substr(found, found_t - found);
    std::stringstream paths(name);
    std::string       line;
    while(getline(paths, line, ' '))
        this->_indexes_location.push_back(line);
}

Location::~Location()
{

}