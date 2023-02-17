#include "Webserver.hpp"

int main( void )
{
    Webserver mainServer;

    std::cout << mainServer.getSocket() << std::endl;
    return ( 0 );
}