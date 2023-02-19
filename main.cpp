#include "Server.hpp"
#include "Configfile.hpp"
#include "Server.hpp"


int main(int ac, char **av)
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
    }
    catch(const char *str)
    {
        std::cout <<str << '\n';
    }

} 