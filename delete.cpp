#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <sstream>  
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <filesystem>

using namespace std;

void	removeContent(const std::string &path)
{
	DIR *dir;

	struct dirent *opt;
	if((dir = opendir(path.c_str())) != NULL)
	{
		while((opt = readdir(dir)) != NULL)
        {
            DIR *dir1;
            std::string teek = (path + "/" + std::string(opt->d_name)).c_str();
            if((dir1= opendir(teek.c_str())) != NULL && strcmp(opt->d_name, ".") && strcmp(opt->d_name, ".."))
                removeContent(teek);
			if(strcmp(opt->d_name, ".") && strcmp(opt->d_name, ".."))
            {
                std::string lok = (path + "/" + std::string(opt->d_name)).c_str();
                int st = remove(lok.c_str());
                if(st == 0)
                    cout<<"removed : "<<lok<<std::endl;
                else
                    cout<<"Not removed : "<<lok<<std::endl;
            }
	    }
    }
}
int main()
{
    removeContent("pk");
    return 0;
}