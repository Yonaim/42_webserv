#include "utils/file.hpp"
#include <cstdio>
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

int ft_open(const std::string &path, const char *mode)
{
	FILE *streamptr = fopen(path.c_str(), mode);
	if (!streamptr)
		return (-1);
	return (fileno(streamptr));
}
