#include "HTTPReq.hpp"
#include "utils.hpp"
#include <exception>
#include <iostream>

using http_msg::HTTPReq;
using http_msg::str_vec_map_t;

/**
 *                	ALL		HEAD	TRAILER		CHUNK
 * start-line		OK		OK		KO			KO
 * header			OK		OK		OK			KO
 * body				OK		OK(X)	OK(X)		OK
 *
 * OK(X) : 해당 부분이 그냥 없어야 OK
 **/

HTTPReq::HTTPReq(std::string const &req)
	: _src(req), _offset(0), _type(NONE), _method(-1)
{
	if (isChunkType())
	{
		parseChunk();
		_type = CHUNK;
	}
	else
	{
		parseStartLine();
		// 성공 -> ALL | HEAD
		// 실패 -> TRAILER
		parseHeader();
		// 다 header 파싱하고
		checkHeader();
		// 어떤 header가 있는지에 따라 type 정하기
		// 지금 all이랑 head && Trasfer-Encoding 헤더가 있다 -> HEAD
		//                                          없다 -> ALL
		// 지금 TRAILER && Transfer-Encoding 헤더가 있다. -> 오류
		// content-length
		parseBody();
	}
}

bool HTTPReq::isChunkType()
{
}

// 결정 가능한 type: ALL | HEAD
void HTTPReq::parseStartLine()
{
	/* line 파싱 */
	std::string line = strBeforeSep(_src, CRLF, _offset);
	if (line == "" || line[0] == ' ')
		throw(std::runtime_error("failed to parse request line."));

	/* 공백 기준 split*/
	std::vector<std::string> tokens = split(line, ' ');
	if (tokens.size() != 3)
	{
		_type = TRAILER;
		return;
	}

	/* method index 구하기 */
	for (int i = 0; i < kMethodCount; i++)
	{
		if (tokens.at(0) == kMethodStr[i])
		{
			_method = i;
			break;
		}
	}
	if (_method == -1)
	{
		_type = TRAILER;
		return;
	}

	/* uri, version 파싱 */
	_uri = components->at(1);
	_version = components->at(2);
	delete components; // split으로 인한 동적 할당 해제
}

void HTTPReq::checkHost()
{
	str_vec_map_t::iterator key_itr = _header.find("Host");

	if (key_itr == _header.end() || key_itr->second.size() == 0)
		throw(std::runtime_error("host error"));
}

bool HTTPReq::hasChunkedVal()
{
	str_vec_map_t::iterator key_itr = _header.find("Transfer-Encoding");

	if (key_itr == _header.end())
		return (false);
	std::vector<std::string>::iterator val_itr = key_itr->second.begin();
	for (; val_itr != key_itr->second.end(); ++val_itr)
	{
		if (*val_itr == "chunked")
			return (true);
	}
	return (false);
}

// 결정 가능한 type: HEAD, ALL, TRAILER
void HTTPReq::parseHeader()
{
	str_t line;

	str_t key;
	size_t key_end_idx;

	str_t value;
	str_vec_t vec_value;
	str_map_t::iterator itr;
	std::string line;
	std::string key;
	size_t key_end_idx;
	std::string value;
	std::vector<std::string> vec_value;
	str_vec_map_t::iterator itr;

	while (true)
	{
		/* line 파싱 */
		line = strBeforeSep(_src, CRLF, _offset);
		if (line == "")
			break;

		/* key 파싱 */
		key_end_idx = 0;
		key = strBeforeSep(line, ":", key_end_idx);
		if (key == "" || hasSpace(key))
			throw(std::runtime_error("failed to parse header fields."));

		/* value 파싱 */
		while (std::isspace(line[key_end_idx]))
			++key_end_idx;
		while (true)
		{
			value = strBeforeSep(line, ",", key_end_idx);
			if (value == "")
				break;
			vec_value.push_back(strtrim(value, " "));
		}
		value = strtrim(std::string(&line[key_end_idx]), " ");
		if (value != "")
			vec_value.push_back(value);

		/* key가 있다면, 붙여넣기 */
		key_itr = _header.find(key);
		if (key_itr == _header.end())
			_header[key] = vec_value;
		else
			key_itr->second.insert(key_itr->second.end(), vec_value.begin(),
								   vec_value.end());
		vec_value.clear();
	}
}
// TODO: trailer에 있으면 안되는 헤더 필드 있는지 확인할 것
// TODO: Content-Length 있는지 확인할 것

void HTTPReq::checkHeader()
{
	/* Host 헤더 있는지 확인 */
	checkHost();
	/* Transfer-Encoding: chunked가 있는지 확인*/
	if (_type == TRAILER)
	{
		if (_header.find("Transfer-Encoding") != _header.end()
			|| _header.find("Content-Length") != _header.end()
			|| _header.find("Trailer") != _header.end())
			throw(
				std::runtime_error("Trailer must not have Transfer-Encoding"));
	}
	else
	{
		if (hasHeaderVal("Transfer-Encoding", "chunked"))
			_type = HEAD;
		else
		{
			_type = ALL;
			if (_method == POST
				&& _header.find("Content-Length") == _header.end())
				throw(std::runtime_error("need Content-Length Header Field"));
		}
	}
}

// body는 Content-Length만큼 읽어야 한다.
void HTTPReq::parseBody()
{
	if (_type != ALL)
	{
		if (_offset != _src.length())
			throw(std::runtime_error("must not have entity body"));
		else
			return;
	}

	switch (_method)
	{
	case (GET):
	case (DELETE): {
		if (_offset != _src.length())
			throw(std::runtime_error("given method must not have the body."));
		break;
	}
	case (POST): {
		if (_offset +)
			_body = _src.substr(_offset, _src.length() - _offset);
	}
	}
}

void HTTPReq::parseChunk()
{
}

// getter
int HTTPReq::getMethod() const
{
	return (_method);
}

std::string const &HTTPReq::getUri() const
{
	return (_uri);
}

std::string const &HTTPReq::getVersion() const
{
	return (_version);
}

str_vec_map_t const &HTTPReq::getHeader() const
{
	return (_header);
}

str_vec_map_t::const_iterator HTTPReq::getHeaderVal(
	std::string const &key) const
{
	return (_header.find(key));
}

std::string const &HTTPReq::getBody() const
{
	return (_body);
}

bool HTTPReq::hasHeaderVal(std::string const &key, std::string const &val) const
{
	str_vec_map_t::const_iterator key_itr = _header.find(key);
	if (key_itr == _header.end())
		return (false);

	std::vector<std::string>::const_iterator val_itr = key_itr->second.begin();
	for (; val_itr != key_itr->second.end(); ++val_itr)
	{
		if (*val_itr == val)
			return (true);
	}
	return (false);
}
