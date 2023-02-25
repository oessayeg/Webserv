#include "ParseConfigfile/Configfile.hpp"



int main(int ac, char **av)
{
    std::string file;
    try
    {
        Configfile Config;
        if (av[1])
        {
            file = av[1];
            Config.check_errors(file);
            std::ifstream infile;
            infile.open(av[1]);
            std::string content = Config.get_contentfile(infile);
            Config.parse_configfile(content);
            std::vector<Serverblock>      servers;
            std::vector<Location>      locations;
            std::vector<std::string>      out;
            servers = Config.get_serverblocks();
            locations = servers[0].get_locationblocks();
            out = locations[0].get_acceptlist_location();
            for(int i = 0; i <out.size(); i++)
            {
                std::cout<<out[i]<<std::endl;
            }
        }

    }
    catch(OurException &e)
    {
        std::cout<<e.what()<<std::endl;
    }
    catch(const char *str)
    {
        std::cout<<str<<std::endl;
    }
    
}
