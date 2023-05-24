#include "../const_values.hpp"
#include "HTTP/Request.hpp"
#include "utils/string.hpp"
#include <iostream>

using namespace HTTP;

int Request::consumeStartLine(std::string &buffer)
{
	// [메소드][SP][URI][SP][HTTP-version][CRLF]

	/* line 파싱 */
	const size_t crlf_pos = buffer.find(CRLF);

	if (crlf_pos == std::string::npos)
	{
		_logger << __func__ << ": buffer doesn't have CRLF" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	const std::string start_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	_logger << __func__ << ": start line is \"" << start_line << "\""
			<< async::debug;

	if (crlf_pos == 0)
	{
		_logger << __func__ << ": buffer's first line is empty"
				<< async::warning;
		throwException(CONSUME_EXC_EMPTY_LINE);
	}
	if (start_line[0] == ' ')
	{
		_logger << __func__ << ": buffer's first character is space"
				<< async::warning;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	/* 공백 기준 split */
	std::vector<std::string> tokens = split(start_line, ' ');
	if (tokens.size() != 3)
	{
		_logger << __func__ << ": token count mismatch" << async::warning;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	{
		_logger << __func__ << ": split into ";
		for (size_t i = 0; i < tokens.size(); i++)
			_logger << "\"" << tokens[i] << "\" ";
		_logger << async::debug;
	}

	/* method index 구하기 */
	for (BidiMap<std::string, int>::const_iterator it = METHOD.begin();
		 it != METHOD.end(); it++)
	{
		if (tokens[0] == it->first)
		{
			_method_string = tokens[0];
			_method = it->second;
			break;
		}
	}

	_logger << __func__ << ": method index is " << _method << async::debug;
	if (_method == METHOD_NONE)
	{
		_logger << __func__ << ": invalid method" << async::warning;
		throwException(CONSUME_EXC_INVALID_VALUE);
	}

	/* uri, version 파싱 */
	_uri = tokens[1];
	// TODO: uri와 query_string 파싱
	_version_string = tokens[2];

	{
		_logger << __func__ << ": URI: \"" << _uri << "\"" << async::verbose;
		_logger << __func__ << ": version: \"" << _version_string << "\""
				<< async::verbose;
		_logger << __func__ << ": buffer result in :\"" << buffer << "\""
				<< async::debug;
	}
	return (RETURN_TYPE_OK);
}

int Request::consumeHeader(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		_logger << __func__ << ": buffer doesn't have CRLF" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	const std::string header_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	_logger << __func__ << ": header line: " << header_line << async::debug;
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		_logger << __func__ << ": header line only has CRLF (end of header)"
				<< async::debug;
		_logger << __func__ << ": buffer result in :\"" << buffer << "\""
				<< async::debug;
		return (RETURN_TYPE_OK);
	}

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
	{
		_logger << __func__ << ": header line has no colon" << async::warning;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}

	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
	{
		_logger << __func__ << ": header name has space" << async::warning;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, ",");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
	{
		*it = strtrim(*it, LWS);
		_logger << __func__ << ": new value \"" << *it << "\"" << async::debug;
	}

	/* key가 있다면, 붙여넣기 */
	if (!_header.hasValue(name))
	{
		_logger << __func__ << ": assign to new header" << async::debug;
		_header.assign(name, values);
	}
	else
	{
		_logger << __func__ << ": insert to existing header" << async::debug;
		_header.insert(name, values);
	}

	return (RETURN_TYPE_IN_PROCESS);
}

int Request::consumeBody(std::string &buffer)
{
	if (buffer.size() < static_cast<size_t>(_content_length))
	{
		_logger << __func__ << ": not enough buffer size" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	_body = consumestr(buffer, _content_length);
	_logger << __func__ << ": body result in :\"" << _body << "\""
			<< async::debug;
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;
	return (RETURN_TYPE_OK);
}

int Request::consumeChunk(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		_logger << __func__ << ": buffer doesn't have CRLF" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	const size_t content_length = strtol(buffer.c_str(), NULL, 16);
	_logger << __func__ << ": content length " << content_length
			<< async::verbose;
	if (content_length < 0)
	{
		_logger << __func__ << ": negative content length of chunk"
				<< async::warning;
		throwException(CONSUME_EXC_INVALID_VALUE);
	}

	// buffer.size() >= 숫자가 적힌 줄 길이 + content_length + CRLF_LEN이면 ok
	if (buffer.size() < crlf_pos + CRLF_LEN + content_length + CRLF_LEN)
	{
		_logger << __func__ << ": not enough buffer size" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	consumestr(buffer, crlf_pos + CRLF_LEN);
	_content_length += content_length;
	_body.append(consumestr(buffer, content_length));
	_logger << __func__ << ": body result in :\"" << _body << "\""
			<< async::debug;
	if (consumestr(buffer, CRLF_LEN) != CRLF)
	{
		_logger << __func__ << ": chunk must end with CRLF" << async::warning;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;

	if (content_length == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}

int Request::consumeTrailer(std::string &buffer)
{
	/**  헤더라인 파싱하기  **/
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		_logger << __func__ << ": buffer doesn't have CRLF" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	const std::string header_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	_logger << __func__ << ": header line: " << header_line << async::debug;
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		_logger << __func__ << ": header line only has CRLF (end of header)"
				<< async::debug;
		return (RETURN_TYPE_OK);
	}

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
	{
		_logger << __func__ << ": header line has no colon" << async::warning;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}

	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
	{
		_logger << __func__ << ": header name has space" << async::warning;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}

	bool found_name = false;
	for (std::vector<std::string>::iterator it = _trailer_values.begin();
		 it != _trailer_values.end(); it++)
	{
		if (*it == name)
		{
			found_name = true;
			_trailer_values.erase(it);
			break;
		}
	}
	if (found_name == false)
	{
		_logger << __func__ << ": Trailer header doesn't have " << name
				<< async::warning;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, ",");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
	{
		*it = strtrim(*it, LWS);
		_logger << __func__ << ": new value \"" << *it << "\"" << async::debug;
	}

	/** key가 있다면, 붙여넣기 **/
	if (!_header.hasValue(name))
	{
		_logger << __func__ << ": assign to new header" << async::verbose;
		_header.assign(name, values);
	}
	else
	{
		_logger << __func__ << ": insert to existing header" << async::verbose;
		_header.insert(name, values);
	}

	if (_trailer_values.size() == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}
