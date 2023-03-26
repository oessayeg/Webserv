#include "Serverblock.hpp"

Serverblock::Serverblock():_ip(0), _port(0),_countbodysize(0), _countlisten(0),_count_location(0),_body_size(10)
{

}

void        Serverblock::block_is_empty(const std::string &block)
{
    size_t found = block.find_first_not_of(" \t\f\v\n\r");
    if (found == std::string::npos)
        throw EmptyError("'Server_Block' is Empty");
}

Serverblock::Serverblock(const Serverblock &opt)
{
    *this = opt;
}

Serverblock &Serverblock::operator=(const Serverblock &opt)
{
    if(this != &opt)
    {
        this->_port = opt._port;
        this->_ip = opt._ip;
        this->_body_size = opt._body_size;
        this->_countlisten = opt._countlisten;
        this->_countbodysize = opt._countbodysize;
        this->_count_location = opt._count_location;
        this->_found = opt._found;
        this->_location = opt._location;
        this->_error_page = opt._error_page;
        this->socketNeeds = opt.socketNeeds;
    }
    return (*this);
}



Serverblock::Serverblock(std::string &block):_countbodysize(0), _countlisten(0),_count_location(0),_body_size(10)
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
        if(line[0] == '#')
            continue ;
        else if (line.substr(0, 6) == "listen")
            set_port_and_ip(line.substr(6));
        else if (line.substr(0, 9) == "body_size")
            set_body_size(line.substr(9));
        else if(line.substr(0, 11) == "server_name")
            set_server_name(line.substr(11));
        else if (line.substr(0, 8) == "location")
        {
            location_block = line + "\n";
            while(getline(outfile, line))
            {
                location_block += line + "\n";
                if (line.find("}") != std::string::npos)
                {
                    size_t find;
                    size_t found  = line.find_first_not_of("  \t\f\v\n");
                    std::string name = line.substr(found);
                    if(name.length() != 1 && (find = name.find_first_not_of(" \t\f\v\n")) == std::string::npos)
                        throw SyntaxError("Invalid close bracket");
                    else
                        break;
                }
            }
            _location.push_back(Location(location_block));
            this->_count_location++;
        }
        else if(line.substr(0, 10) == "error_page")
            set_error_page(line.substr(10));
        else
            throw NotFoundError("'Server block' :  name is not valid");
        check_duplicate();
    }
    check_valid_config();
}

void        Serverblock::check_valid_config()
{
    if (this->_countlisten == 0  || this->_count_location == 0)
        throw LogicError("configfile Error");
}

void        Serverblock::check_valid_value(const std::string &buffer, std::string &value)
{
    size_t found = buffer.find_first_not_of("  \t\f\v\n\r;");
    if(found == std::string::npos)
        throw LogicError("'name' should have a value");
    size_t found_t = buffer.find_first_of(";");
    if (found_t == std::string::npos)
        throw SyntaxError("value Should be Closed By ';'");
    value = buffer.substr(found, found_t - found + 1);
}

void        Serverblock::check_value_arg(const std::string &value)
{
    size_t  found = value.find_first_not_of(" \t\f\v\n\r");
    size_t found_t = value.find_first_of(" \t\f\v\n\r;", found);
    size_t found_tt = value.find_first_not_of(" \t\f\v\n\r;", found_t);
    if (found_tt != std::string::npos)
        throw LogicError("Invalid Arg");
}

bool Serverblock::is_Number(const std::string &str)
{
    for(int i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == 0)
            return false;
    return true;
}

bool Serverblock::isNumberIp(const std::string &str)
{
    for(int i = 0; i < str.length(); i++)
        if (std::isdigit(str[i]) == 0 && str[i] != '.')
            return false;
    return true;
}

bool        Serverblock::check_valid_port(std::string &port)
{
    size_t found = port.find_first_of(" \t\f\v\n\r;");

    port = port.substr(0, found);
    if (!isNumberIp(port))
        throw LogicError("Invalid Port");
    unsigned int num = atoi(port.c_str());
    if (num > USHRT_MAX || num <= 0)
        throw LogicError("Your Port Is Out Of Range '" + port + "'");
    return (true);
}

bool        Serverblock::check_valid_ip(const std::string    &ip)
{
    if(!isNumberIp(ip))
        throw LogicError("Inavlie value of ip '" + ip + "'");
    return (1);
}

bool        Serverblock::check_valid_listen(const std::string &value, std::string &ip, std::string &port)
{
    std::string str[2];
    int i = 0;
    size_t found = value.find_first_of(":");
    if (found == std::string::npos)
    {
        ip = "127.0.0.1";
        port = value;
        return (true);
    }
    size_t find_port = value.find_first_not_of(" \t\f\v\n\r;:", found + 1);
    if (find_port == std::string::npos)
        throw LogicError("Invalid arg");
    str[0] = value.substr(0, find_port - 1);
    if(str[0].empty())
        throw LogicError("Invalid value of ip ' '");
    size_t find_next = value.find_first_of(" \t\f\v\n\r;", find_port);
    str[1]  = value.substr(find_port, find_next - find_port);
    ip = str[0];
    if(ip == "localhost")
        ip = "127.0.0.1";
    port = str[1];
    return (true);
}


void        Serverblock::set_port_and_ip(const std::string &line)
{
    std::string ip;
    std::string port;
    std::string value;
    check_valid_value(line, value);
    check_value_arg(value);
    if (check_valid_listen(value, ip, port))
    {
        std::cout<<"ip : "<<ip<<std::endl;
        if (check_valid_ip(ip) && check_valid_port(port))
        {
            this->_port = atoi(port.c_str());
            this->_ip = inet_addr(ip.c_str());
        }
        else
            throw LogicError("'listen' invalid value");
    }
    this->_countlisten++;
}

void    Serverblock::set_body_size(const std::string &body_size)
{
    std::string value;
    check_valid_value(body_size, value);
    check_value_arg(value);
    size_t found = value.find_first_of(" \t\f\v\n\r;");
    value = value.substr(0, found);
    if (!is_Number(value))
        throw LogicError("'Body_Size' Value not Valid");
    std::stringstream ff(value);
    ff >> this->_body_size;
    if(this->_body_size > 2500)
        throw LogicError("'Body_Size' value : '" + ff.str() + "' is big");
    this->_countbodysize++;
}

void        Serverblock::check_valid_status_code(const std::string &key)
{
    if (!is_Number(key))
        throw LogicError("Status code not valid : '" + key + "'");
    int num = atoi(key.c_str());
    if (num < 100 || num > 999)
        throw LogicError("Status code is out of range : '" + key + "'");
}

void        Serverblock::set_error_page(const std::string &line)
{
    std::string value;
    std::string key;
    std::string data;
    check_valid_value(line, value);
    size_t found = value.find_first_not_of(" \t\f\v\n\r");
    size_t found_next = value.find_first_of(" \t\f\v\n\r", found + 1);
    if(found_next == std::string::npos)
        throw LogicError("'error_page' invalid value");
    key = value.substr(found, found_next - found);
    check_valid_status_code(key);
    found  = value.find_first_not_of(" \t\f\v\n\r;", found_next + 1);
    if(found == std::string::npos)
        throw LogicError("'error_page' invalid value");
    found_next = value.find_first_of(" \t\f\v\n\r;", found + 1);
    data = value.substr(found, found_next - found);
    found = value.find_first_not_of(" \t\f\v\n\r;", found_next + 1);
    if(found != std::string::npos)
        throw LogicError("'error_page' invalid value");
    _error_page[atoi(key.c_str())] = data;
}

void                        Serverblock::set_server_name(const std::string &line)
{
    std::string value;

    check_valid_value(line, value);
    check_value_arg(value);
     size_t found = value.find_first_of(" \t\f\v\n\r;");
    std::string name = value.substr(0, found);
    this->_serverName = name;

}

void        Serverblock::check_duplicate()
{
    if (this->_countbodysize > 1 || this->_countlisten > 1 )
        throw SyntaxError("'Duplicate' name");
}

int         Serverblock::get_port() const
{
    return (this->_port);
}

in_addr_t Serverblock::get_ip() const
{
    return (this->_ip);
}

size_t        Serverblock::get_body_size() const
{
    return(this->_body_size);
}

std::list<Location> Serverblock::get_locationblocks() const
{
    return(this->_location);
}
std::string                 Serverblock::get_server_name()
{
    return (this->_serverName);
}

void	replaceString(std::string &str, const std::string &oldstring, const std::string &newString)
{
	size_t startPos = 0;
	startPos = str.find(oldstring, startPos);
	if(startPos != std::string::npos)
		str.replace(startPos, oldstring.length(), newString);
}

std::list<Location>::iterator	Serverblock::ifUriMatchLocationBlock(std::list<Location> &list, const std::string &uri)
{
	bool			isFound = false;
	std::string	matcheLocation = "";
	std::list<std::string>::iterator it1;
	std::list<Location>::iterator it = list.begin();
	std::list<Location>::iterator returnBlock;
	std::string str = uri;

	std::list<std::string> my_list;
	for(; it != list.end(); ++it)
	{
		my_list = it->get_path_location();
		it1 = my_list.begin();
		for(; it1 != my_list.end(); ++it1)
		{
        str = uri;
			if(uri.find(*it1)  == 0 && (*it1).size() > matcheLocation.size())
			{
				matcheLocation = *it1;
				isFound = true;
				replaceString(str, *it1, it->get_root_location());
				returnBlock = it;
				returnBlock->_currentRoot  = str;
			}
		}
	}
	if(isFound == true)
		return (returnBlock);
	return (list.end());
}

Serverblock::~Serverblock()
{
    
}