#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/select.h>


class Webserver
{
    private :
        int _socket;
        int _port;

    public :
        Webserver( void );
        Webserver( const Webserver &rhs );
        Webserver &operator=( const Webserver &rhs );
        Webserver( const int port );
        ~Webserver( void );
        
        // Getters
        int getSocket( void ) const;

        // Socket handlers
        void createSocket( void );
};