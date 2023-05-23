#include "utils/file.hpp"
#include <dirent.h>

bool isDirectory(const std::string &path)
{
	DIR *dir = opendir(path.c_str());
	if (dir)
	{
		closedir(dir);
		return (true);
	}
	else
	{
		return (false);
	}
}
