#include <unistd.h>
#include "MainHeader.hpp"


int main()
{
    MimeTypes path;

    std::cout<<path.getContentType("lolo/teeek/lolo")<<std::endl;
}


