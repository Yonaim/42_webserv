#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <sstream>
#include <string>
#include <vector>

// parsing
bool hasSpace(const std::string &key);
bool isHTTPSpace(const char c);
bool isUnsignedIntStr(const std::string &str);
void passLWS(std::string &str);
void strtrim(std::string &str, const std::string &charset);
std::vector<std::string> split(const std::string &s, const char c);
std::vector<std::string> split(const std::string &s, const std::string &sep);
char *duplicateStr(const std::string &str);

// returns substring from start to (until).
inline std::string getfrontstr(const std::string &str, const size_t until)
{
	return (str.substr(0, until));
}

// trims the end of string except `until` characters.
inline void trimbackstr(std::string &str, const size_t until)
{
	str.erase(until, std::string::npos);
}

// returns substring from `from` to end.
inline std::string getbackstr(const std::string &str, const size_t from)
{
	return (str.substr(from, str.size() - from));
}

// trims `from` characters from the front of str.
inline void trimfrontstr(std::string &str, const size_t from)
{
	str.erase(0, from);
}

inline std::string consumestr(std::string &str, const size_t from)
{
	std::string buf = str.substr(0, from);
	trimfrontstr(str, from);
	return (buf);
}

inline std::string getExtension(const std::string &filename)
{
	size_t pos = filename.rfind('.');
	if (pos != std::string::npos)
		return (filename.substr(pos + 1));
	return ("");
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

template <typename T> T toHexNum(const std::string &str)
{
	T num;
	std::istringstream ss(str);

	if (!(ss >> std::hex >> num))
	{
		throw(std::invalid_argument("String is not representing hex: " + str));
	}

	char remaining;
	if (ss >> remaining)
	{
		throw(std::invalid_argument(
			"Leftover character after str to hex convertion: " + str));
	}
	return (num);
}

#endif
