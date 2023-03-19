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
        std::string                              _upload_dir;
        std::list<std::string>                 _indexes_location;
        int                                      _countroot;
        int                                      _count_allow_methode;
        int                                      _count_auto_index;
        int                                      _count_return;
        bool                                     _isThereCgi;
        bool                                     _isThereRedirection;
        bool                                     _supportUpload;
        std::list<std::string>                 _path_location;
    public:
        std::list<std::string>                 _accept_list;
        std::string                              _redirection[2];
        std::string                              _currentRoot;
        Location();
        Location(std::string &location);
        Location(const Location &);
        Location &operator=(const Location&);


        void                        set_path_location(std::string           path_location);
        void                        set_root_location(std::string           root_location);
        void                        set_autoindex_location(std::string      index_location);
        void                        set_accept_list_location(std::string    accept_list);
        void                        set_indexes_location(std::string        indexes_location);
        void                        set_redirection(std::string             redirection);
        void                        set_upload_dir(std::string              path);
        void                        set_cgi(std::string                     path);

        bool                        get_autoindex() const;
        std::string                 get_root_location() ;
        std::list<std::string>      get_acceptlist_location() const;
        std::list<std::string>      get_indexes_location() const;
        std::list<std::string>      get_path_location() ;
        bool                        get_cgi();
        bool                        get_isThereRedirection();

        void                        init_list();
        void                        check_duplicate();
        void                        check_valid_value(std::string buffer, std::string &value);
        void                        check_valid_status_code(std::string key);
        void                        check_value_arg(std::string value);

        bool                        is_Number(std::string  str);
        ~Location();
};

#endif