#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <sstream>
#include <string>

// parsing
bool hasSpace(const std::string &key);
bool isHTTPSpace(const char c);
bool isUnsignedIntStr(const std::string &str);
void passLWS(std::string &str);
std::string strtrim(std::string str, std::string charset);
std::vector<std::string> split(std::string const &s, char const c);
std::vector<std::string> split(std::string const &s, std::string const &sep);
std::string strBeforeSep(const std::string &be_parsed, const std::string &sep,
						 size_t &start);
std::string getfrontstr(const std::string &str, size_t until);
void trimbackstr(std::string &str, size_t until);
std::string getbackstr(const std::string &str, size_t from);
void trimfrontstr(std::string &str, size_t from);
std::string getExtension(const std::string &filename);

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

template <typename T> T toNum(std::string &str)
{
	T num;
	std::istringstream ss(str);

	if (!(ss >> num))
	{
		throw std::invalid_argument("Invalid input: '" + str + "'");
	}

	char remaining;
	if (ss >> remaining)
	{
		throw std::invalid_argument("Invalid input: '" + str + "'");
	}
	return (num);
}

template <typename T> T toNum(const std::string &str)
{
	T num;
	std::istringstream ss(str);

	if (!(ss >> num))
	{
		throw std::invalid_argument("Invalid input: '" + str + "'");
	}

	char remaining;
	if (ss >> remaining)
	{
		throw std::invalid_argument("Invalid input: '" + str + "'");
	}
	return (num);
}

#endif
