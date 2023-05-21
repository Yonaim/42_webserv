#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "utils/string.hpp"

bool hasSpace(const std::string &str)
{
	for (size_t i = 0; str[i]; ++i)
		if (std::isspace(str[i]))
			return (true);
	return (false);
}

bool isHTTPSpace(const char c)
{
	return (c == ' ' || c == '\t');
}

bool isUnsignedIntStr(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return (false);
	return (true);
}

// [CRLF] 1*( SP | HT )
void passLWS(std::string &str)
{
	size_t after_lws = 0;

	while (str[after_lws])
	{
		if (str[after_lws] != 's' || str[after_lws] != '\t')
			break;
		++after_lws;
	}
	str = str.substr(after_lws, str.size() - after_lws);
}

std::vector<std::string> split(std::string const &s, char const c)
{
	std::vector<std::string> words;
	size_t offset = 0;
	size_t end_idx;

	while (offset < s.length())
	{
		if (s[offset] != c)
		{
			if ((end_idx = s.find(c, offset)) == std::string::npos)
				end_idx = s.length();
			std::string word = s.substr(offset, end_idx - offset);
			words.push_back(word);
			offset = end_idx;
		}
		offset++;
	}
	return (words);
}

static size_t _findFirstSep(std::string const &s, std::string const &sep,
							size_t offset)
{
	while (offset < s.length())
	{
		if (sep.find(s[offset]) != std::string::npos)
			break;
		offset++;
	}
	return (offset);
}

std::vector<std::string> split(std::string const &s, std::string const &sep)
{
	std::vector<std::string> words;
	size_t offset = 0;
	size_t end_idx;

	while (offset < s.length())
	{
		if (sep.find(s[offset]) == std::string::npos)
		{
			end_idx = _findFirstSep(s, sep, offset);
			std::string word = s.substr(offset, end_idx - offset);
			words.push_back(word);
			offset = end_idx;
		}
		offset++;
	}
	return (words);
}

std::string strBeforeSep(const std::string &be_parsed, const std::string &sep,
						 size_t &start)
{
	std::string result;
	size_t end;

	end = be_parsed.find(sep, start);
	if (end == std::string::npos)
		return ("");
	result = be_parsed.substr(start, end - start);
	start = end + sep.size();
	return (result);
}

std::string strtrim(std::string str, std::string charset)
{
	size_t start;
	size_t offest;

	start = 0;
	while (str[start] && strchr(charset.c_str(), str[start]))
		++start;
	offest = start;
	while (str[offest] && !strchr(charset.c_str(), str[offest]))
		++offest;
	return (str.substr(start, offest - start));
}

std::string uintToStr(size_t num)
{
	size_t tem = num;
	size_t length = 0;
	std::string num_str;

	if (num == 0)
		return ("0");
	while (tem > 0)
	{
		length++;
		tem /= 10;
	}
	num_str.resize(length, '\0');
	tem = num;
	while (tem > 0)
	{
		num_str[--length] = tem % 10 + '0';
		tem /= 10;
	}
	return (num_str);
};

// returns substring from start to (until).
std::string getfrontstr(const std::string &str, size_t until)
{
	return (str.substr(0, until));
}

// trims the end of string except `until` characters.
void trimbackstr(std::string &str, size_t until)
{
	str = str.substr(0, until);
}

// returns substring from `from` to end.
std::string getbackstr(const std::string &str, size_t from)
{
	return (str.substr(from, str.size() - from));
}

// trims `from` characters from the front of str.
void trimfrontstr(std::string &str, size_t from)
{
	str = str.substr(from, str.size() - from);
}
