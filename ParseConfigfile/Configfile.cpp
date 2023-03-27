#include "Configfile.hpp"
Configfile::Configfile()
{

}

Configfile::Configfile(const Configfile &opt)
{
    *this = opt;
}

Configfile & Configfile::operator=(const Configfile &opt)
{
    if(this != &opt)
    {
        this->server = opt.server;
    }
    return (*this);
}

std::string     Configfile::get_contentfile(std::ifstream &infile)
{
    std::stringstream buffer;

    buffer << infile.rdbuf();
    return (buffer.str());
}

void    Configfile::parse_configfile(std::string &configfile)
{
    while(configfile != "")
    {
        size_t found = configfile.find_first_not_of(" \t\f\v\n\r");
        if (found == std::string::npos)
            break;
        else if(configfile.compare(found, 6, "server") == 0)
        {
            int pos = 1;
            size_t start = found + 6;
            size_t first_bracket = configfile.find_first_not_of(" \t\f\v\n\r", start);
            if (configfile[first_bracket] != '{')
                throw SyntaxError("'block' should start with '}'");
            size_t last_bracket = configfile.find_first_of("{}", first_bracket + 1);
            while (last_bracket != std::string::npos && pos != 0)
            {
                if(configfile[last_bracket] == '{')
                    pos++;
                else
                    pos--;
                if (pos != 0)
                    last_bracket = configfile.find_first_of("{}", last_bracket + 1);
            }
            if (last_bracket == std::string::npos)
                throw SyntaxError("'block' block should be closed by '}'");
            std::string body = configfile.substr(first_bracket + 1, last_bracket  - first_bracket - 1);
            server.push_back(Serverblock(body));
            configfile.erase(0, last_bracket + 1);
        }
        else
            throw SyntaxError("Identifier Name");
    }
}



std::list<Serverblock>      &Configfile::get_serverblocks()
{
    return (this->server);
}