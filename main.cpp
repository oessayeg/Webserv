#include "MainHeader.hpp"
#include "Webserver.class.hpp"

std::string getFileN( char **av )
{
	std::string file;

	file = DEFAULT;
	if (av[1])
		file = av[1];
	return file;
}

void checkArgs( int ac, char **av )
{
	std::ifstream infile;
	std::string file;

	file = DEFAULT;
	if (ac > 2)
		throw "Invalid Arguments";
	if (av[1])
		file = av[1];
	infile.open(file);
	if (!infile.is_open())
		throw NotFoundError("Does Not Exist");
	else
		infile.close();
}

int main(int ac, char **av)
{
    try
    {
        Webserver mainServer;
        Configfile Config;
		std::string content;
        std::ifstream infile;

		checkArgs(ac, av);
		infile.open(getFileN(av));
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