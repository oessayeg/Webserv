#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <list>
#include "Exception.hpp"

class Location
{
    private:
        bool                                     _auto_index;
        std::string                              _root;
        std::string                              _redirection[2];
        std::vector<std::string>                 _accept_list;
        std::vector<std::string>                 _path_location;
        std::vector<std::string>                 _indexes_location;
    public:
        Location();
        Location(std::string &location);


        void        set_path_location(std::string           path_location);
        void        set_root_location(std::string           root_location);
        void        set_autoindex_location(std::string      index_location);
        void        set_accept_list_location(std::string    accept_list);
        void        set_indexes_location(std::string        indexes_location);
        void        set_redirection(std::string             redirection);

        bool                        get_autoindex() const;
        std::string                 get_root_location() const;
        std::vector<std::string>    get_acceptlist_location() const;
        std::vector<std::string>    get_indexes_location() const;

        void                        init_list();
        ~Location();
};

#endif