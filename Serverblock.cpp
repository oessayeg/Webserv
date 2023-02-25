#include "Serverblock.hpp"

Serverblock::Serverblock():_ip(0), _port(0)
{

}

void        Serverblock::block_is_empty(std::string &block)
{
    size_t found = block.find_first_not_of(" \t\f\v\n\r");
    if (found == std::string::npos)
        throw EmptyError("'Server_Block' is Empty");
}


Serverblock::Serverblock(std::string &block)
{
    block_is_empty(block);
    std::string location_block;
    std::stringstream outfile(block);
    std::string line;
    while (getline(outfile, line))
    {
        size_t found_first_char = line.find_first_not_of(" \t\f\v\n{\r");
        size_t found_last_char = line.find_last_not_of(" \t\f\v\n\r}");
        if (found_first_char == std::string::npos)
            continue;
        line = line.substr(found_first_char, found_last_char - found_first_char + 1);
        if (line.substr(0, 6) == "listen")
            set_port_and_ip(line.substr(6));
        else if (line.substr(0, 9) == "body_size")
            set_body_size(line.substr(9));
        else if (line.substr(0, 4) == "root")
            set_root(line.substr(4));
        else if (line.substr(0, 5) == "index")
            set_index(line.substr(5));
        else if (line.substr(0, 8) == "location")
        {
            location_block = line + "\n";
            while(getline(outfile, line))
            {
                location_block += line + "\n";
                if (line.find("}") != std::string::npos)
                    break;
            }
            _location.push_back(Location(location_block));
        }
    }
}

void        Serverblock::check_valid_value(std::string buffer, std::string &value)
{
    size_t found = buffer.find_first_not_of("  \t\f\v\n\r");
    if(found == std::string::npos)
        throw SyntaxError("'name' should have a value");
    size_t found_t = buffer.find_first_of(";");
    if (found_t == std::string::npos)
        throw SyntaxError("value Should be Closed By ';'");
    value = buffer.substr(found, found_t - found + 1);
}

void        Serverblock::check_value_arg(std::string value)
{
    size_t  found = value.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = value.find_first_of(" \t\f\v\n\r;", found);
    size_t found_tt = value.find_first_not_of(" \t\f\v\n\r;", found_t);
    if (found_tt != std::string::npos)
        throw SyntaxError("Invalid Arg");
}

bool Serverblock::is_Number(std::string  str)
{
    for(int i = 0; i < str.length(); i++)
    {
        if (std::isdigit(str[i]) == 0)
            return false;
    }
    return true;
}

bool        Serverblock::check_valid_port(std::string &port)
{
    size_t found = port.find_first_of(" \t\f\v\n\r;");

    port = port.substr(0, found);
    if (!is_Number(port))
        throw SyntaxError("Invalid Port");
    unsigned int num = atoi(port.c_str());
    if (num > USHRT_MAX)
        throw SyntaxError("Your Ip Is Out Of Range '" + port + "'");
    return (true);
}

void             Serverblock::check_valid_numIp(std::string &value)
{
    if (value.length() > 3)
        throw SyntaxError("Invalid Ip");
    if(!is_Number(value))
        throw SyntaxError("Invalid Ip");
    int num = atoi(value.c_str());
    if (num > 255)
        throw SyntaxError("Ip is not in valid Range");
}


bool        Serverblock::check_valid_ip(std::string &ip)
{
    int count;

    count = 0;
    if (ip != "0")
    {
        size_t found = ip.find_first_of(".");
        size_t  found_t = ip.find_first_of(".");
        if (found == std::string::npos)
            return(true);
        while (found_t != std::string::npos)
        {
            count++;
            found_t = ip.find_first_of(".", found_t + 1);
        }
        if(count != 3)
            throw SyntaxError("Invalid Ip");
        if (found != std::string::npos)
        {
            std::stringstream outfile(ip);
            std::string line;
            while (getline(outfile, line, '.'))
                check_valid_numIp(line);
        }
    }
    return (true);
}

bool        Serverblock::check_valid_listen(std::string value, std::string &ip, std::string &port)
{
  
    std::string str[2];
    int i = 0;
    size_t found = value.find(":");
    if (found == std::string::npos)
    {
        ip = "0";
        port = value;
        return (true);
    }
    size_t find_port = value.find_first_not_of(" \t\f\v\n\r;:", found + 1);
    if (find_port == std::string::npos)
        throw SyntaxError("Invalid arg");
    std::stringstream ff(value);
    std::string line;
    while (getline(ff, line, ':'))
    {
        str[i] = line;
        ++i;
    }
    ip = str[0];
    port = str[1];
    return (true);
}


void        Serverblock::set_port_and_ip(std::string line)
{
    std::string ip;
    std::string port;
    std::string value;
    check_valid_value(line, value);
    check_value_arg(value);
    if (check_valid_listen(value, ip, port))
    {
        if (check_valid_ip(ip) && check_valid_port(port))
        {
            this->_port = atoi(port.c_str());
            this->_ip = ip;
        }
    }
}

void    Serverblock::set_body_size(std::string body_size)
{
    std::string value;
    check_valid_value(body_size, value);
    check_value_arg(value);
    size_t found = value.find_first_of(" \t\f\v\n\r;");
    value = value.substr(0, found);
    if (!is_Number(value))
        throw SyntaxError("Body Size Value Not Valid");
    this->_body_size = atol(value.c_str());
}

void    Serverblock::set_root(std::string root)
{
    std::string value;
    check_valid_value(root, value);
    check_value_arg(value);
    size_t found = value.find_first_of(" \t\f\v\n\r;");
    value = value.substr(0, found);
    if (value[0] == '/')
        value = value.substr(1);
    if(access(value.c_str(), F_OK) != 0)
        throw NotFoundError("'root' Folder Is Not Found");
}

void    Serverblock::set_index(std::string indexes)//3andak tnsa after ; 
{
    std::string value;
    check_valid_value(indexes, value);
    size_t found = value.find_first_of(";");
    value = value.substr(0, found);
    std::stringstream ff(value);
    std::string       line;
    while (getline(ff, line, ' '))
        this->_indexes.push_back(line);
}


int         Serverblock::get_port() const
{
    return (this->_port);
}

std::string Serverblock::get_ip() const
{
    return (this->_ip);
}

long        Serverblock::get_body_size() const
{
    return(this->_body_size);
}

std::string Serverblock::get_root()  const
{
    return (this->_root);
}

std::vector<Location> Serverblock::get_locationblocks() const
{
    return(this->_location);
}
