#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <vector>

std::vector<std::string> *split(std::string const &s, char const c);
std::vector<std::string> *split(std::string const &s, std::string const &sep);


#endif