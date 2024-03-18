#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int autoindex(const char *directory_name, std::string &res) {
    DIR* directory = opendir(directory_name);
	std::string fullPath;
	res = "<!DOCTYPE html>\n<html lang=\"fr\">\n\t<head>\n\t<meta charset=\"UTF-8\">\n\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t<title>Index des fichiers</title>\n\t</head>\n<body>\n\t<body>\n\t\t<ul>\n";
    struct dirent *entry;
    if (directory == NULL) {
        std::cerr << "error open" << std::endl;
        return (0);
    }
    while ((entry = readdir(directory)) != NULL)
	{
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
            fullPath = std::string(directory_name) + "/" + entry->d_name;
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) != -1)
			{
                if (S_ISDIR(fileStat.st_mode))
                    res+="\t\t\t<li><a href=\"" + fullPath + "\"> " + entry->d_name + " </a></li>\n";
				else //S_ISREG(fileStat.st_mode))
                    res+="\t\t\t<li><a href=\"" + fullPath + "\"> " + entry->d_name + " </a></li>\n";
            }
        }
    }
	res += "\t\t</ul>\n\t</body>";
    closedir(directory);
	return (1);
}
//prend en parametre le path du dossier depuis le root
int main(int, char **av) {
    const char* directory_path = av[1];
	std::string	dest;
	if (!autoindex(directory_path, dest))
		return (0);
    std::cout<<dest<<std::endl;
    return 0;
}

