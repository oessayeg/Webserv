#include "MainHeader.hpp"

int main( void )
{
    try
    {
            std::cout << "Hello from server" << std::endl;
    }
    catch( const char *msg )
    {
        std::cerr << msg << std::endl;
    }
    
    return (0);
}