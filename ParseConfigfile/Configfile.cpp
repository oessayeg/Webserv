#include "Configfile.hpp"
Configfile::Configfile()
{

}

std::string     Configfile::get_contentfile(std::ifstream &infile)
{
    std::stringstream buffer;

    buffer << infile.rdbuf();
    return (buffer.str());
}

void            Configfile::check_errors(std::string    &namefile)
{
    std::ifstream file;

    file.open(namefile);
    if (!file)
        throw FileError("does Not Exist");
    size_t found = namefile.find(".conf");
    if(found != std::string::npos)
    {
        size_t start = found + 5;
        size_t find = namefile.find_first_not_of(" \t\f\v\n\r", start);
        if (find != std::string::npos)
             throw FileError("'extension should be '.conf'");
    }
    else
         throw FileError("extension should be '.conf'");
}

void    Configfile::skip_comments(std::string& Configfile) {

    std::string result;
    bool found_comment = false;

    for (std::string::iterator it = Configfile.begin(); it != Configfile.end(); ++it) {
        char c = *it;
        if (found_comment) {
            if(c == ';')
                throw SyntaxError("Invalid comment");
            if (c == '\n') {
                found_comment = false;
            }
        } else {
            if (c == '#') {
                found_comment = true;
            } else {
                result += c;
            }
        }
    }
    Configfile = result;
}
void    Configfile::parse_configfile(std::string &configfile)
{
    skip_comments(configfile);
    while(configfile != "")
    {
        size_t found = configfile.find_first_not_of(" \t\f\v\n\r");
        if (found == std::string::npos)
            break;
        else if(configfile.compare(found, 6, "server") == 0)
        {
            int pos = 1;
            size_t start = found + 6;
            size_t first_bracket = configfile.find_first_not_of(" \t\f\v\n\r", start);
            if (configfile[first_bracket] != '{')
                throw SyntaxError("'block' should be start white '}'");
            size_t last_bracket = configfile.find_first_of("{}", first_bracket + 1);
            while (last_bracket != std::string::npos && pos != 0)
            {
                if(configfile[last_bracket] == '{')
                    pos++;
                else
                    pos--;
                if (pos != 0)
                    last_bracket = configfile.find_first_of("{}", last_bracket + 1);
            }
            if (last_bracket == std::string::npos)
                throw SyntaxError("'block' block should be closed by '}'");
            std::string body = configfile.substr(first_bracket + 1, last_bracket  - first_bracket - 1);
            server.push_back(Serverblock(body));
            configfile.erase(0, last_bracket + 1);
        }
        else
            throw SyntaxError("Identifier Name");
    }
}



std::vector<Serverblock>      Configfile::get_serverblocks()
{
    return (this->server);
}