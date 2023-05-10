#include "HttpReq.hpp"
#include "HttpMsgException.hpp"
#include "utils.hpp"
#include <iostream>

using http_msg::HttpReq;
using http_msg::str_map_t;
using http_msg::str_t;
using http_msg::str_vec_t;

HttpReq::HttpReq(str_t const &req)
	: _src(req), _offset(0), _method(-1), _status(0)
{
}

// parsing
bool HttpReq::parse()
{
	try
	{
		parseStartLine();
		parseHeader();
		parseBody();
		return (true);
	}
	catch (HttpReqException &e)
	{
		e.deleteAll();
		std::cerr << e.what() << std::endl;
		_status = 400;
		return (false);
	}
}

void HttpReq::parseStartLine()
{
	/* line 파싱 */
	str_t line = strBeforeSep(_src, CRLF, _offset);
	if (line == "" || line[0] == ' ')
		throw(HttpReqException(ERR_START_LINE_PARSING));

	/* 공백 기준 split*/
	str_vec_t *components = split(line, ' ');
	if (!components || components->size() != 3)
		throw(HttpReqException(ERR_START_LINE_PARSING, components));

	/* method index 구하기 */
	for (int i = 0; i < method::kCount; i++)
	{
		if (components->at(0) == method::kTypeStr[i])
		{
			_method = i;
			break;
		}
	}
	if (_method == -1)
		throw(HttpReqException(ERR_START_LINE_PARSING, components));

	/* uri, version 파싱 */
	_uri = components->at(1);
	_version = components->at(2);
	delete components; // split으로 인한 동적 할당 해제
}

// 여러 줄 헤더 처리
void HttpReq::parseHeader()
{
	str_t line;

	str_t  key;
	size_t key_end_idx;

	str_t     value;
	str_vec_t vec_value;

	while (true)
	{
		/* line 파싱 */
		line = strBeforeSep(_src, CRLF, _offset);
		if (line == "")
			break;

		/* key와 value 파싱 */
		key_end_idx = 0;
		key = strBeforeSep(line, ":", key_end_idx);
		if (key == "" || hasSpace(key))
			throw(HttpReqException(ERR_HEADER_PARSING));
		// value 파싱
		while (std::isspace(line[key_end_idx]))
			++key_end_idx;
		while (true)
		{
			value = strBeforeSep(line, ",", key_end_idx);
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
}

void HttpReq::parseBody()
{
	switch (_method)
	{
	case (method::kGet):
	case (method::kDelete): {
		if (_offset != _src.length())
			throw(HttpReqException(ERR_BODY_PARSING));
		break;
	}
	case (method::kPost): {
		const size_t end_idx = _src.find_last_of(CRLF, _offset);
		if (end_idx == std::string::npos || end_idx != _src.length() - CRLF_LEN)
			throw(HttpReqException(ERR_BODY_PARSING));
		_body = _src.substr(_offset, end_idx - _offset);
	}
	}
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

str_t const &HttpReq::getVersion() const
{
	return (_version);
}

str_map_t const &HttpReq::getHeader() const
{
	return (_header);
}

str_map_t::const_iterator HttpReq::getHeaderVal(str_t const &key) const
{
	return (_header.find(key));
}

str_t const &HttpReq::getBody() const
{
	return (_body);
}
