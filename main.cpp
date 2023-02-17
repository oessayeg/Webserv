#include "Webserver.hpp"

int main( void )
{
    Webserver mainServer(80);

    mainServer.createSocket();
    std::cout << mainServer.getSocket() << std::endl;
    return ( 0 );
}