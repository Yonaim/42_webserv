#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

// parsing
bool                     hasSpace(const std::string &key);
bool                     passLWS(std::string &str);
std::string              strtrim(std::string str, std::string charset);
std::vector<std::string> split(std::string const &s, char const c);
std::vector<std::string> split(std::string const &s, std::string const &sep);
std::string strBeforeSep(const std::string &be_parsed, const std::string &sep,
						 size_t &start);
std::string getfrontstr(const std::string &str, size_t until);
void        trimbackstr(std::string &str, size_t until);
std::string getbackstr(const std::string &str, size_t from);
void        trimfrontstr(std::string &str, size_t from);

// conversion
std::string uintToStr(size_t num);

template <typename T> std::string toStr(T num, T num2)
{
	std::ostringstream ss;

	ss << num << num2;
	return (ss.str());
}

template <typename T> std::string toStr(T num)
{
	std::ostringstream ss;

	ss << num;
	return (ss.str());
}

template <typename T> T toNum(std::string str)
{
	T                  num;
	std::istringstream ss(str);

	ss >> num;
	return (num);
}

#endif