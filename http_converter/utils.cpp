#include <cstring>
#include <iostream>
#include <string>
#include <vector>

bool hasSpace(const std::string &key)
{
	for (size_t i = 0; key[i]; ++i)
		if (std::isspace(key[i]))
			return (true);
	return (false);
}

std::vector<std::string> *split(std::string const &s, char const c)
{
	std::vector<std::string> *words = new std::vector<std::string>;
	size_t                    offset = 0;
	size_t                    end_idx;

	while (offset < s.length())
	{
		if (s[offset] != c)
		{
			if ((end_idx = s.find(c, offset)) == std::string::npos)
				end_idx = s.length();
			std::string word = s.substr(offset, end_idx - offset);
			words->push_back(word);
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

std::vector<std::string> *split(std::string const &s, std::string const &sep)
{
	std::vector<std::string> *words = new std::vector<std::string>;
	size_t                    offset = 0;
	size_t                    end_idx;

	while (offset < s.length())
	{
		if (sep.find(s[offset]) == std::string::npos)
		{
			end_idx = _findFirstSep(s, sep, offset);
			std::string word = s.substr(offset, end_idx - offset);
			words->push_back(word);
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
	size_t      end;

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

// int main(void)
// {
// 	std::string               str = "abc,,,, d,eee,ffasdf, f a,w,e,,f,annn,adf";
// 	std::string               sep = " ,";
// 	std::vector<std::string> *splitted;

// 	splitted = split(str, ',');
// 	std::cout << "=== split(str, ',') ===" << std::endl;
// 	for (int i = 0; i < splitted->size(); i++)
// 	{
// 		std::cout << i << ": " << splitted->at(i) << std::endl;
// 	}
// 	delete splitted;
// 	splitted = split(str, sep);
// 	std::cout << "=== split(str, \" ,\") ===" << std::endl;
// 	for (int i = 0; i < splitted->size(); i++)
// 	{
// 		std::cout << i << ": " << splitted->at(i) << std::endl;
// 	}
// 	delete splitted;
// }
