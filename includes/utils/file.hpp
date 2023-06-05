#ifndef FILE_HPP
#define FILE_HPP

#include <stdexcept>
#include <string>

bool isDirectory(const std::string &path);
size_t getFileSize(const std::string &path);

#endif
