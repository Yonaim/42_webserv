#include "utils/string.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

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
	trimfrontstr(str, after_lws);
}

std::vector<std::string> split(const std::string &s, const char c)
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

static size_t _findFirstSep(const std::string &s, const std::string &sep,
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

std::vector<std::string> split(const std::string &s, const std::string &sep)
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

void strtrim(std::string &str, const std::string &charset)
{
	size_t start;
	size_t offset;

	start = 0;
	while (start < str.length() && strchr(charset.c_str(), str[start]))
		++start;
	trimfrontstr(str, start);
	offset = 0;
	while (offset < str.length() && !strchr(charset.c_str(), str[offset]))
		++offset;
	trimbackstr(str, offset);
}
