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
	if (crlf_pos == 0)
	{
		_logger << __func__ << ": buffer's first line is empty"
				<< async::verbose;
		_error_offset = 0;
		throwException(CONSUME_EXC_EMPTY_LINE);
	}
	if (buffer[0] == ' ')
	{
		_logger << __func__ << ": buffer's first character is space"
				<< async::verbose;
		_error_offset = 0;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	const std::string start_line = getfrontstr(buffer, crlf_pos);
	_logger << __func__ << ": start line is \"" << start_line << "\""
			<< async::debug;

	/* 공백 기준 split */
	std::vector<std::string> tokens = split(start_line, ' ');
	if (tokens.size() != 3)
	{
		_logger << __func__ << ": token count mismatch" << async::verbose;
		_error_offset = start_line.size();
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	_logger << __func__ << ": split into ";
	for (size_t i = 0; i < tokens.size(); i++)
		_logger << "\"" << tokens[i] << "\" ";
	_logger << async::debug;

	/* method index 구하기 */
	for (BidiMap<std::string, int>::const_iterator it = METHOD.begin(); it != METHOD.end(); it++)
	{
		if (tokens[0] == it->first)
		{
			_method = it->second;
			break;
		}
	}

	_logger << __func__ << ": method index is " << _method << async::debug;
	if (_method == METHOD_NONE)
	{
		_logger << __func__ << ": invalid method" << async::verbose;
		_error_offset = tokens[0].size();
		throwException(CONSUME_EXC_INVALID_VALUE);
	}

	/* uri, version 파싱 */
	_uri = tokens[1];
	_version_string = tokens[2];
	_logger << __func__ << ": URI: \"" << _uri << "\"" << async::debug;
	_logger << __func__ << ": version: \"" << _version_string << "\""
			<< async::debug;

	trimfrontstr(buffer, start_line.size() + CRLF_LEN);

	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;
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

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		_logger << __func__ << ": header line only has CRLF (end of header)"
				<< async::debug;
		trimfrontstr(buffer, CRLF_LEN);
		_logger << __func__ << ": buffer result in :\"" << buffer << "\""
				<< async::debug;
		return (RETURN_TYPE_OK);
	}

	const std::string header_line = getfrontstr(buffer, crlf_pos);
	_logger << __func__ << ": header line: " << header_line << async::debug;

	/* name 파싱 */
	size_t key_end_idx = 0;
	std::string name = strBeforeSep(header_line, ":", key_end_idx);
	if (name == "" || hasSpace(name))
	{
		_logger << __func__ << ": header has invalid name" << async::verbose;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}
	_logger << __func__ << ": header name is " << name << async::debug;

	/* value 파싱 */
	while (std::isspace(header_line[key_end_idx]))
		++key_end_idx;

	std::vector<std::string> vec_values;
	while (true)
	{
		std::string value = strBeforeSep(header_line, ",", key_end_idx);
		if (value == "")
			break;
		vec_values.push_back(strtrim(value, " "));
		_logger << __func__ << ": push value " << vec_values.back()
				<< async::debug;
	}
	std::string value = strtrim(std::string(&header_line[key_end_idx]), " ");
	if (value != "")
	{
		vec_values.push_back(value);
		_logger << __func__ << ": push value " << vec_values.back()
				<< async::debug;
	}

	/* key가 있다면, 붙여넣기 */
	if (!_header.hasValue(name))
	{
		_logger << __func__ << ": assign to new header" << async::debug;
		_header.assign(name, vec_values);
	}
	else
	{
		_logger << __func__ << ": insert to existing header" << async::debug;
		_header.insert(name, vec_values);
	}

	trimfrontstr(buffer, header_line.size() + CRLF_LEN);
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;

	return (RETURN_TYPE_IN_PROCESS);
}

int Request::consumeBody(std::string &buffer)
{
	// size_t content_length = 0; // TODO: 헤더 맵에서 Content-length 파싱해오기

	if (buffer.size() < static_cast<size_t>(_content_length))
	{
		_logger << __func__ << ": not enough buffer size" << async::debug;
		return (RETURN_TYPE_AGAIN);
	}

	_body = getfrontstr(buffer, _content_length);
	_logger << __func__ << ": body result in :\"" << _body << "\""
			<< async::debug;
	trimfrontstr(buffer, _content_length);
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
	const int content_length = strtol(buffer.c_str(), NULL, 16);
	_logger << __func__ << ": content length " << content_length
			<< async::debug;
	// TODO: content length 0일때 마지막 청크 처리
	if (content_length < 0)
	{
		_logger << __func__ << ": negative content length of chunk"
				<< async::verbose;
		throwException(CONSUME_EXC_INVALID_VALUE);
	}
	// buffer.size() >= 숫자 길이 + content_length + CRLF_LEN * 2이면 ok
	if (buffer.size() < crlf_pos + content_length + CRLF_LEN * 2)
	{
		return (RETURN_TYPE_AGAIN);
	}
	_body.append(buffer.substr(crlf_pos + CRLF_LEN, content_length));
	_logger << __func__ << ": body result in :\"" << _body << "\""
			<< async::debug;
	if (buffer.substr(crlf_pos + CRLF_LEN + content_length, CRLF_LEN) != CRLF)
	{
		_logger << __func__ << ": chunk must end with CRLF" << async::verbose;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	trimfrontstr(buffer, crlf_pos + CRLF_LEN * 2 + content_length);
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
	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		_logger << __func__ << ": header line only has CRLF (end of header)"
				<< async::debug;
		trimfrontstr(buffer, CRLF_LEN);
		_logger << __func__ << ": buffer result in :\"" << buffer << "\""
				<< async::debug;
		return (RETURN_TYPE_OK);
	}

	const std::string header_line = getfrontstr(buffer, crlf_pos);
	_logger << __func__ << ": header line: " << header_line << async::debug;
	
	/** name 파싱 **/
	size_t key_end_idx = 0;
	const std::string name = strBeforeSep(header_line, ":", key_end_idx);
	if (name == "" || hasSpace(name))
	{
		_logger << __func__ << ": header has invalid name" << async::verbose;
		_error_offset = key_end_idx;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	_logger << __func__ << ": header name is " << name << async::debug;

	std::vector<std::string>::iterator iter = _trailer_values.begin();
	bool found_name = false;
	for (; iter != _trailer_values.end(); ++iter)
	{
		if (*iter == name)
		{
			found_name = true;
			_trailer_values.erase(iter);
			break;
		}
	}
	if (found_name == false)
	{
		_logger << __func__ << ": Trailer header doesn't have " << name
				<< async::verbose;
		_error_offset = key_end_idx;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}
	passLWS(buffer);

	/** value 파싱 **/
	std::vector<std::string> vec_values;
	while (true)
	{
		std::string value = strBeforeSep(header_line, ",", key_end_idx);
		if (value == "")
			break;
		vec_values.push_back(strtrim(value, " "));
		_logger << __func__ << ": push value " << vec_values.back()
				<< async::debug;
	}
	std::string value = strtrim(std::string(&header_line[key_end_idx]), " ");
	if (value != "")
	{
		vec_values.push_back(value);
		_logger << __func__ << ": push value " << vec_values.back()
				<< async::debug;
	}

	/** key가 있다면, 붙여넣기 **/
	if (!_header.hasValue(name))
	{
		_logger << __func__ << ": assign to new header" << async::debug;
		_header.assign(name, vec_values);
	}
	else
	{
		_logger << __func__ << ": insert to existing header" << async::debug;
		_header.insert(name, vec_values);
	}

	trimfrontstr(buffer, header_line.size() + CRLF_LEN);
	_logger << __func__ << ": buffer result in :\"" << buffer << "\""
			<< async::debug;

	if (_trailer_values.size() == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}
