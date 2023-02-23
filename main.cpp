#include "MainHeader.hpp"

// Temporary function
std::list < blocks > giveList( void )
{
	std::list < blocks > list;
	blocks ins1;
	ins1.index.push_back("index1.html");
	ins1.port = 8080;
	ins1.ip = 0;
	ins1.root = "./";

	list.push_back(ins1);
	ins1.index[0] = "index2.html";
	ins1.port = 9090;
	ins1.ip = 0;
	ins1.root = "./";
	list.push_back(ins1);

	return list;
}


// Main
int main( void )
{
	try
	{
		Webserver mainServer;

		mainServer.setServerBlocks(giveList());
		mainServer.createSockets();
		while (1)
		{
			mainServer.setReadyFds();
			mainServer.readAndRespond();
		}
	}
	catch( const char *msg )
	{
		std::cerr << msg << std::endl;
	}
	
	return (0);
}