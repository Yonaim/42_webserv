#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef std::string              str_t;
typedef std::vector<std::string> str_vec_t;

const std::string CRLF = "\n";
const std::string SP = " ";

std::string                _src = "Host: www.joes.com\nAccept: *\n\n";
size_t                     _offset = 0;
std::map<str_t, str_vec_t> _header;

str_t parseBeforeSep(const str_t &be_parsed, const str_t &sep, size_t &start)
{
	str_t  result;
	size_t end;

	end = be_parsed.find(sep, start);
	if (end == str_t::npos)
		return ("");
	result = be_parsed.substr(start, end - start);
	start = end + sep.size();
	return (result);
}

std::string	strtrim(str_t str, str_t charset)
{
	size_t	start;
	size_t	offest;

	start = 0;
	while (str[start] && strchr(charset.c_str(), str[start]))
		++start;
	offest = start;
	while (str[offest] && !strchr(charset.c_str(), str[offest]))
		++offest;
	return (str.substr(start, offest - start));
}

bool	has_space(const std::string &key)
{
	for (size_t i = 0; key[i]; ++i)
		if (std::isspace(key[i]))
			return (true);
	return (false);
}

// 여러 줄 헤더 처리
bool parseHeader()
{
	str_t  line;

	str_t  key;
	size_t key_end_idx;

	str_t     value;
	str_vec_t vec_value;

	while (true)
	{
		/* line 파싱 */
		line = parseBeforeSep(_src, CRLF, _offset);
		if (line == "" || line == CRLF) // CRLF면 빈 줄이니까 break
			break;

		/* key와 value 파싱 */
		key_end_idx = 0;
		key = parseBeforeSep(line, ":", key_end_idx);
		if (key == "" || has_space(key))
			return (false);
		// value 파싱
		while (std::isspace(line[key_end_idx]))
			++key_end_idx;
		while (true)
		{
			value = parseBeforeSep(line, ",", key_end_idx);
			if (value == "")
				break;
			vec_value.push_back(strtrim(value, " "));
		}
		value = strtrim(str_t(&line[key_end_idx]), " ");
		if (value != "")
			vec_value.push_back(value);
		_header[key] = vec_value;
		vec_value.clear();
	}
	return (true);
}

int main(void)
{
	std::getline(std::cin, _src, '_');
	std::cout << "----------------------------" << std::endl;
	for (size_t i = 0; _src[i]; ++i)
	{
		if (_src[i] == '\r')
			std::cout << "\\r";
		else if (_src[i] == '\n')
			std::cout << "\\n";
		else
			std::cout << _src[i];
	}
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;
	if (parseHeader() == false)
		std::cout << "Error" << std::endl;

	std::map<str_t, str_vec_t>::iterator itr = _header.begin();
	for (; itr != _header.end(); ++itr)
	{
		std::cout << "size : " << itr->second.size() << std::endl;
		std::cout << itr->first << " | ";
		for (size_t i = 0; i < itr->second.size(); ++i)
		{
			std::cout << "* " << itr->second[i] << "*";
			if (i != itr->second.size() - 1)
				std::cout << ",";
		}
		std::cout << std::endl;
	}
	std::cout << "offset : " << _offset << std::endl;
	return (0);
}
