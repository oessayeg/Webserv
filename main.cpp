#include "MainHeader.hpp"
#include "Webserver.class.hpp"

int main(int ac, char **av)
{
    Webserver mainServer;
    std::string file;

    try
    {
        if(ac > 2)
        throw  "Invalid argument";
        Configfile Config;
        file = DEFAULT;
        if (av[1])
            file = av[1];
        std::ifstream infile;
        infile.open(file);
        if(!infile.is_open())
            throw NotFoundError("Do Not Exist");
        std::string content = Config.get_contentfile(infile);
        Config.parse_configfile(content);
		mainServer.setServerBlocks(Config.get_serverblocks());
		mainServer.createSockets();
		while (1)
		{
			mainServer.setReadyFds();
			mainServer.readAndRespond();
		}

    }
    catch (OurException &e)
    {
        std::cout<<e.what()<<std::endl;
    }
    catch (const char *str)
    {
        std::cout<<str<<std::endl;
    }
   	catch ( std::exception &rhs1 )
	{
		std::cout << rhs1.what() << std::endl;
	}
    catch ( std::string &s )
    {
        std::cout << s << std::endl;
    }
}