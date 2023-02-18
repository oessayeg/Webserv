#include "Webserver.hpp"

int main( void )
{
    try
    {
        Webserver mainServer(80, 4000);

        mainServer.createSocket();
        mainServer.bindAndListen();
        mainServer.acceptConnections();
    }
    catch ( const char *msg )
    {
        std::cout << msg << std::endl;
    }

    return ( 0 );
}