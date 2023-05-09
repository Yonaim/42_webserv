#include "HttpReq.hpp"
#include "utils.hpp"

using http_msg::HttpReq;
using http_msg::str_t;
using http_msg::str_vec_t;

HttpReq::HttpReq(str_t const &req) : _src(req), _method(-1), _offset(0)
{
}

// parsing
bool HttpReq::parse()
{
	try
	{
		return (parseStartLine() && parseHeader() && parseBody());
	}
	catch (HttpReqException &e)
	{
		e.deleteAll();
		std::cerr << e.what() << std::endl;
		_status = e.getErrrcode();
		return (false);
	}
}

bool HttpReq::parseStartLine()
{
	const size_t line_end_idx = _src.find(CRLF, _offset);
	if (line_end_idx == std::string::npos)
		throw(HttpReqException(400, NULL));
	std::string line = _src.substr(0, line_end_idx);
	str_vec_t  *components = split(_src.substr(0, line_end_idx), ' ');
	if (!components || components->size() != 3)
		throw(HttpReqException(400, components));
	for (int i = 0; i < http_msg::method::kCount; i++)
	{
		if (components->at(0) == http_msg::method::kTypeStr[i])
		{
			_method = i;
			break;
		}
	}
	if (_method == -1)
		throw(HttpReqException(400, components));
	_uri = components->at(1);
	_version = components->at(2);
	_offset = line_end_idx + 2;
	return (true);
}

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

std::string strtrim(str_t str, str_t charset)
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

bool has_space(const std::string &key)
{
	for (size_t i = 0; key[i]; ++i)
		if (std::isspace(key[i]))
			return (true);
	return (false);
}

// 여러 줄 헤더 처리
bool HttpReq::parseHeader()
{
	str_t line;

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

bool HttpReq::parseBody()
{
	const size_t end_idx = _src.find(CRLF, _offset);
	if (end_idx == std::string::npos || end_idx != _src.length() - 1)
		return (processError(400));
	_body = _src.substr(_offset, end_idx - _offset);
}

// getter
int HttpReq::getMethod() const
{
	return (_method);
}

str_t const &HttpReq::getUri() const
{
	return (_uri);
}

str_vec_t const &HttpReq::getHeaderVal(str_t const &key) const
{
}

str_t const &HttpReq::getBody() const
{
	return (_body);
}

// error
bool HttpReq::processError(int status)
{
	_status = status;
	return (false);
}