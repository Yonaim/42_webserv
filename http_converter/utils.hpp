#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

// parsing
bool                      hasSpace(const std::string &key);
std::string               strtrim(std::string str, std::string charset);
std::vector<std::string> split(std::string const &s, char const c);
std::vector<std::string> split(std::string const &s, std::string const &sep);
std::string strBeforeSep(const std::string &be_parsed, const std::string &sep,
						 size_t &start);

// conversion
std::string uintToStr(size_t num);

#endif