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
        std::list<std::string>                 _accept_list;
        std::list<std::string>                 _path_location;
        std::list<std::string>                 _indexes_location;
        int                                      _countroot;
        int                                      _count_allow_methode;
        int                                      _count_auto_index;
        int                                      _count_return;
    public:
        Location();
        Location(std::string &location);


        void                        set_path_location(std::string           path_location);
        void                        set_root_location(std::string           root_location);
        void                        set_autoindex_location(std::string      index_location);
        void                        set_accept_list_location(std::string    accept_list);
        void                        set_indexes_location(std::string        indexes_location);
        void                        set_redirection(std::string             redirection);

        bool                        get_autoindex() const;
        std::string                 get_root_location() const;
        std::list<std::string>      get_acceptlist_location() const;
        std::list<std::string>      get_indexes_location() const;
        std::list<std::string>      get_path_location() const;

        void                        init_list();
        void                        check_duplicate();
        void                        check_valid_value(std::string buffer, std::string &value);
        ~Location();
};

#endif