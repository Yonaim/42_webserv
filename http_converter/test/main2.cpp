#include <iostream>
#include <string>
#include <vector>

typedef std::string              str_t;
typedef std::vector<std::string> str_vec_t;

const std::string CRLF = "\r\n";
const std::string SP = " ";
const int         kCount = 3;
const std::string kTypeStr[3] = {"GET", "POST", "DELET"};

const std::string _src = "GET /THIS.	US/ADF HTTP/1.1\r\n";

int         _method;
std::string _version;
std::string _uri;
int         _status = -1;
int         _offset = 0;

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

bool processError(int status)
{
	_status = status;
	return (false);
}

bool parseStartLine()
{
	const size_t line_end_idx = _src.find(CRLF, _offset);
	if (line_end_idx == std::string::npos)
		return (processError(400));

	std::string line = _src.substr(0, line_end_idx);
	str_vec_t  *components = split(line, ' ');
	if (!components || components->size() != 3)
		return (processError(400));

	for (int i = 0; i < kCount; i++)
	{
		if (components->at(0) == kTypeStr[i])
		{
			_method = i;
			break;
		}
	}
	if (_method == -1)
		return (processError(400));
	_uri = components->at(1);
	_version = components->at(2);
	_offset = line_end_idx + 2;
	return (true);
}

int main(void)
{
	parseStartLine();
	std::cout << "method : " << _method << std::endl;
	std::cout << "uri : " << _uri << std::endl;
	std::cout << "version : " << _version << std::endl;
	std::cout << "offset : " << _offset << std::endl;
	std::cout << "status : " << _status << std::endl;
}
