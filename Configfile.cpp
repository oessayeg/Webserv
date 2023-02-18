#include "Configfile.hpp"
Configfile::Configfile()
{

}

std::string     Configfile::readfile(std::ifstream &infile)
{
    std::stringstream buffer;

    buffer << infile.rdbuf();
    return (buffer.str());
}

void       Configfile::trim_spaces_around(std::string &configfile)
{
    int found = configfile.find_first_not_of(" \n\t\f\v\r{");
    configfile.erase(0, found);
}

void    Configfile::parse_file(std::string &configfile)
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
                throw "'server' block should be start whit '}'";
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
                throw "'server' block should be closed by '}'";
            std::string body = configfile.substr(first_bracket, last_bracket - first_bracket + 1);
            server.push_back(Server(body));
            configfile.erase(0, last_bracket + 1);

        }
        else
            throw "name not found";
    }
}


int                          Configfile::get_size_of_server() const
{
    return (this->server.size());
}

int main(int ac, char *av[])
{
    Configfile op;
    if (ac != 2)
    {
        std::cout<<"invalid arg"<<std::endl;
        return (0);
    }
    try
    {
        std::string configfile;
        std::ifstream infile;
        infile.open(av[1]);
        configfile = op.readfile(infile);
        op.parse_file(configfile);
        std::cout<<op.server[2].get_port()<<std::endl;
    }
    catch(const char *str)
    {
        std::cout <<str << '\n';
    }

}
