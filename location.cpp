#include "location.hpp"

Location::Location():_auto_index(false),_count_auto_index(0),_count_allow_methode(0),_countroot(0),_count_return(0)
{
}

Location::Location(std::string &location):_count_auto_index(0),_count_allow_methode(0),_countroot(0),_count_return(0),_isThereCgi(false)
{
    init_list();
    size_t find = location.find_first_of("/");
    if (find == std::string::npos)
        throw SyntaxError("'Location' Should have a valid path");
    std::stringstream outfile(location);
    std::string line;
    while (getline(outfile, line))
    {
        size_t found_t = line.find_first_not_of(" \t\f\v\n\r{}");
        size_t found_tt = line.find_last_not_of(" \t\f\v\n\r{}");
        if (found_t == std::string::npos)
                continue;
        line = line.substr(found_t, found_tt - found_t + 1);
        if(line[0] == '#')
            continue ;
        else if (line.substr(0, 8) == "location")
            set_path_location(line.substr(8));
        else if (line.substr(0, 4) == "root")
           set_root_location(line.substr(4));
        else if (line.substr(0, 9) == "autoindex")
            set_autoindex_location(line.substr(10));
        else if (line.substr(0, 11) == "accept_list")
            set_accept_list_location(line.substr(11));
        else if(line.substr(0, 5) == "index")
            set_indexes_location(line.substr(5));
        else if(line.substr(0, 6) == "return")
            set_redirection(line.substr(6));
        else if(line.substr(0,10) == "upload_dir")
            set_upload_dir(line.substr(10));
        else if(line.substr(0, 7) == "run_cgi")
            set_cgi(line.substr(7));
        else
            throw NotFoundError("'Location block' Name Not Found");
        check_duplicate();
    }
}

void        Location::check_valid_value(std::string buffer, std::string &value)
{
    size_t found = buffer.find_first_not_of("  \t\f\v\n\r;");
    if(found == std::string::npos)
        throw LogicError("'name' should have a value");
    size_t found_t = buffer.find_first_of(";");
    if (found_t == std::string::npos)
        throw SyntaxError("value Should be Closed By ';'");
    value = buffer.substr(found, found_t - found + 1);
}

void        Location::set_path_location(std::string path_location)//should skipp withspace in path location
{
    size_t find = path_location.find_first_not_of(" \t\f\v\n\r;");
    if(find == std::string::npos)
        throw SyntaxError("Invalid argument in location path");
    std::string name = path_location.substr(find);
    std::stringstream buffer(name);
    std::string line;
    while(getline(buffer, line, ' '))
    {
        if(line[0] != '/')
            throw SyntaxError("'" + line + "' path should start with '/'");
        _path_location.push_back(line);
    }
}

bool Location::is_Number(std::string  str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if (std::isdigit(str[i]) == 0)
            return false;
    }
    return true;
}
 
void        Location::check_value_arg(std::string value)
{
    size_t  found = value.find_first_not_of(" \t\f\v\n\r;");
    size_t found_t = value.find_first_of(" \t\f\v\n\r;", found);
    size_t found_tt = value.find_first_not_of(" \t\f\v\n\r;", found_t);
    if (found_tt != std::string::npos)
        throw LogicError("Invalid Arg");
}

void        Location::set_root_location(std::string root_location)
{
    std::string value;

    check_valid_value(root_location, value);
    check_value_arg(value);
    size_t found = value.find_first_of(" \t\f\v\n\r;");
    std::string name = value.substr(0, found);
    if(name[name.length() - 1] != '/')
        this->_root = name + '/';
    else
        this->_root = name;
}

void    Location::set_accept_list_location(std::string accept_list)
{
    if (!_accept_list.empty())
        _accept_list.clear();
    size_t found = accept_list.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = accept_list.find_first_of(";");
    if (found_t == std::string::npos)
        throw SyntaxError("'location block' accept_list value should be closed by ';'");
    std::string name = accept_list.substr(found, found_t - found);
    std::stringstream list(name);
    std::string line;
    while(getline(list, line, ' '))
        _accept_list.push_back(line);
    this->_count_allow_methode++;
}

void        Location::set_autoindex_location(std::string index_location)
{
    size_t found = index_location.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = index_location.find_first_of(";");
    if (found_t == std::string::npos)
       SyntaxError("'location block' autoindex value should be closed by ';'");
    std::string name = index_location.substr(found, found_t - found);
    found = name.find_first_of(" \t\f\v\n\r");
    found_t = name.find_first_not_of(" \t\f\v\n\r", found);
    if (found_t != std::string::npos)
        throw "invalid auto index";
    if (name  == "on")
        this->_auto_index = true;
    else if (name == "off")
        this->_auto_index = false;
    else
        throw LogicError("'autoindex' : invalid arg");
    this->_count_auto_index++;
}

void        Location::check_valid_status_code(std::string key)
{
    if (!is_Number(key))
        throw LogicError("Status code not valid : '" + key + "'");
    int num = atoi(key.c_str());
    if (num < 100 || num > 999)
        throw LogicError("Status code is out of range : '" + key + "'");
}

void        Location::set_indexes_location(std::string        indexes_location)
{
    size_t found = indexes_location.find_first_not_of("  \t\f\v\n\r");
    size_t found_t = indexes_location.find_first_of(";");
    if (found_t == std::string::npos)
       SyntaxError("'location block' index value should be closed by ';'");
    std::string name = indexes_location.substr(found, found_t - found);
    std::stringstream paths(name);
    std::string       line;
    while(getline(paths, line, ' '))
        this->_indexes_location.push_back(line);
    std::list<std::string>::iterator it = this->_indexes_location.begin();
}


void        Location::check_duplicate()
{
    if (this->_count_allow_methode > 1 || this->_count_auto_index > 1 || this->_countroot > 1 
    || this->_count_return > 1)
        throw SyntaxError("'Location block' Duplicate name");
}

void        Location::set_redirection(std::string             redirection)
{
    std::string value;
    std::string key;
    std::string data;
    check_valid_value(redirection, value);
    size_t found = value.find_first_not_of(" \t\f\v\n\r");
    size_t found_next = value.find_first_of(" \t\f\v\n\r", found + 1);
    if(found_next == std::string::npos)
        throw LogicError("'return' invalid value");
    key = value.substr(found, found_next - found);
    check_valid_status_code(key);
    found  = value.find_first_not_of(" \t\f\v\n\r;", found_next + 1);
    if(found == std::string::npos)
        throw LogicError("'return' invalid value");
    found_next = value.find_first_of(" \t\f\v\n\r;", found + 1);
    data = value.substr(found, found_next - found);
    found = value.find_first_not_of(" \t\f\v\n\r;", found_next + 1);
    if(found != std::string::npos)
        throw LogicError("'return' invalid value");
    _redirection[0] = key;
    _redirection[1] = data;
    this->_count_return++;
}


void        Location::set_upload_dir(std::string             path)
{
    std::string value;

    check_valid_value(path, value);
    check_value_arg(value);
    size_t found = value.find_first_of(" \t\f\v\n\r;");
    std::string name = value.substr(0, found);
    this->_upload_dir = name;
}

void        Location::set_cgi(std::string             path)
{
    std::string value;
    check_valid_value(path, value);
    check_value_arg(value);

    size_t found = value.find_first_of(" \t\f\v\n\r;");
    std::string name = value.substr(0, found);
    if (name  == "on")
        this->_isThereCgi = true;
    else if (name == "off")
        this->_isThereCgi = false;
    else
        throw LogicError("'autoindex' : invalid arg");
}

bool    Location::get_autoindex() const
{
    return (this->_auto_index);
}

void        Location::init_list()
{
    _accept_list.push_back("GET");
    _accept_list.push_back("POST");
    _accept_list.push_back("DELETE");
}

std::list<std::string>    Location::get_acceptlist_location() const
{
    return (this->_accept_list);
}

std::list<std::string>    Location::get_path_location() const
{
    return (this->_path_location);
}

std::list<std::string>    Location::get_indexes_location() const
{
    return (this->_indexes_location);
}

Location::~Location()
{

}