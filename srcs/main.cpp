#include "../includes/MainHeader.hpp"
#include "../includes/Webserver.class.hpp"

int main(int ac, char **av)
{
    try
    {
        Webserver mainServer;
        Configfile Config;
		std::string content;
        std::ifstream infile;

		Utils::checkArgs(ac, av);
		infile.open(Utils::getFileN(av));
        content = Config.get_contentfile(infile);
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
        std::cout << e.what() << std::endl;
    }
    catch (const char *str)
    {
        std::cout << str << std::endl;
    }
   	catch (std::exception &rhs1)
	{
		std::cout << rhs1.what() << std::endl;
	}
    catch (std::string &s)
    {
        std::cout << s << std::endl;
    }
}