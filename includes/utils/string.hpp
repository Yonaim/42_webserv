#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <sstream>
#include <string>

// parsing
bool hasSpace(const std::string &key);
bool isHTTPSpace(const char c);
bool isUnsignedIntStr(const std::string &str);
void passLWS(std::string &str);
void strtrim(std::string &str, const std::string &charset);
std::vector<std::string> split(const std::string &s, const char c);
std::vector<std::string> split(const std::string &s, const std::string &sep);
std::string getfrontstr(const std::string &str, const size_t until);
void trimbackstr(std::string &str, const size_t until);
std::string getbackstr(const std::string &str, const size_t from);
void trimfrontstr(std::string &str, const size_t from);
std::string consumestr(std::string &str, const size_t from);
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
