#include "MainHeader.hpp"
#include "Block.tmp.hpp"

// Temporary function
std::list < Blocks > giveList( void )
{
	std::list < Blocks > list;
	Blocks ins1;
	ins1.index.push_back("index1.html");
	ins1.port = 8080;
	ins1.ip = 0;
	ins1.maxBodySize = 10000;
	ins1.root = "./";

	list.push_back(ins1);
	ins1.index[0] = "index2.html";
	ins1.port = 9090;
	ins1.ip = 0;
	ins1.maxBodySize = 10000;
	ins1.root = "./";
	list.push_back(ins1);

	return list;
}

// Main
int main( void )
{
	
	Webserver mainServer;

	mainServer.setServerBlocks(giveList());
	mainServer.createSockets();
	while (1)
	{
		mainServer.setReadyFds();
		mainServer.readAndRespond();
	}
	return (0);
}