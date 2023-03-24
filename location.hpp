#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <list>
#include <fstream>
#include <sys/stat.h>
#include "Response.class.hpp"
#include "Exception.hpp"

class Location
{
    private:
        bool                                     _auto_index;
        std::string                              _root;
        int                                      _countroot;
        int                                      _count_allow_methode;
        int                                      _count_auto_index;
        int                                      _count_return;
        bool                                     _isThereCgi;
        bool                                     _isThereRedirection;
        std::list<std::string>                   _path_location;
  
    public:
        bool                                     _supportUpload;
        std::string                              _upload_dir;
        std::list<std::string>                   _indexes_location;
        std::list<std::string>                   _accept_list;
        std::string                              _redirection[2];
        std::string                              _currentRoot;
        Location();
        Location(std::string &location);
        Location(const Location &);
        Location &operator=(const Location&);


        void                        set_path_location(const std::string           &path_location);
        void                        set_root_location(const std::string           &root_location);
        void                        set_autoindex_location(const std::string      &index_location);
        void                        set_accept_list_location(const std::string    &accept_list);
        void                        set_indexes_location(const std::string        &indexes_location);
        void                        set_redirection(const std::string             &redirection);
        void                        set_upload_dir(const std::string              &path);
        void                        set_cgi(const std::string                     &path);

        bool                        get_autoindex() const;
        std::string                 get_root_location() ;
        std::list<std::string>      get_acceptlist_location() const;
        std::list<std::string>      get_indexes_location() const;
        std::list<std::string>      get_path_location() ;
        bool                        get_cgi();
        bool                        get_isThereRedirection();

        void                        init_list();
        void                        check_duplicate();
        void                        check_valid_value(const std::string &buffer, std::string &value);
        void                        check_valid_status_code(const std::string &key);
        void                        check_value_arg(const std::string &value);

        bool                        is_Number(const std::string  &str);
        bool                        isMethodAccepted( std::list< Location >::iterator location, const std::string &method ); 
        bool                        checkIfPathExist(const std::string &path);
        bool                        ifRequestUriIsFolder( const std::string &uri);
        bool                        checkIfPathIsValid(const std::string &path, const std::string &uri, Response &resp, const std::string &root);

        ~Location();
};

#endif