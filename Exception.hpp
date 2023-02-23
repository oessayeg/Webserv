# ifndef EXECPTION_HPP
# define EXECPTION_HPP

#include <iostream>
#include <cstring>
class OurException 
{
    public :
        virtual std::string what() const throw() = 0;
   
};

class FileError : public  OurException
{
        private : 
            std::string message;
        public :
        FileError(std::string msg):message(msg){}
        std::string what() const throw()
        {
            std::string name = "the Configfile : ";
            return(name + message);
        }
};

class SyntaxError  : public  OurException
{
        private : 
            std::string message;
        public :
        SyntaxError(std::string msg):message(msg){}
        std::string what() const throw()
        {
            std::string name = "SyntaxError : ";
            return(name + message);
        }
};

class EmptyError  : public  OurException
{
        private : 
            std::string message;
        public :
        EmptyError(std::string msg):message(msg){}
        std::string what() const throw()
        {
            return(message);
        }
};

class NotFoundError  : public  OurException
{
        private : 
            std::string message;
        public :
        NotFoundError(std::string msg):message(msg){}
        std::string what() const throw()
        {
            return(message);
        }
}; 
# endif