#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> *split(std::string const &s, char const c)
{
	std::vector<std::string> *words = new std::vector<std::string>;
	int                       offset = 0;
	int                       end_idx;

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

static size_t find_first_sep(std::string const &s, std::string const &sep,
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
	int                       offset = 0;
	int                       end_idx;

	while (offset < s.length())
	{
		if (sep.find(s[offset]) == std::string::npos)
		{
			end_idx = find_first_sep(s, sep, offset);
			std::string word = s.substr(offset, end_idx - offset);
			words->push_back(word);
			offset = end_idx;
		}
		offset++;
	}
	return (words);
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
