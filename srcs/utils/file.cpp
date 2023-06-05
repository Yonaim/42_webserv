#include "utils/file.hpp"
#include <cerrno>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>

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

size_t getFileSize(const std::string &path)
{
	struct stat buf;

	if (stat(path.c_str(), &buf) != 0)
		throw(std::runtime_error(strerror(errno)));

	return (buf.st_size);
}
