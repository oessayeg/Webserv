#include "Webserver.hpp"

int main( void )
{
    try
    {
        Webserver mainServer(80, 4000);

        mainServer.createSocket();
        mainServer.bindAndListen();
        while (1)
        {
            mainServer.acceptConnections();
            mainServer.parseRequest();
            mainServer.processRequest();
        }
    }
    catch ( const char *msg )
    {
        std::cout << msg << std::endl;
    }

    return ( 0 );
}