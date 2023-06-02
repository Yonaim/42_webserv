#include "HTTP/ParsingFail.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/const_values.hpp"
#include "utils/string.hpp"

using namespace HTTP;

int Request::consumeStartLine(std::string &buffer)
{
	// [메소드][SP][URI][SP][HTTP-version][CRLF]

	/* line 파싱 */
	const size_t crlf_pos = buffer.find(CRLF);

	if (crlf_pos == std::string::npos)
	{
		LOG_DEBUG(__func__ << ": buffer doesn't have CRLF");
		return (RETURN_TYPE_AGAIN);
	}

	const std::string start_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	LOG_DEBUG(__func__ << ": start line is \"" << start_line << "\"");

	if (crlf_pos == 0)
	{
		_logger << async::warning << __func__
				<< ": buffer's first line is empty";
		throw(HTTP::EmptyLineFound());
	}
	if (start_line[0] == ' ')
	{
		_logger << async::warning << __func__
				<< ": buffer's first character is space";
		throw(HTTP::InvalidFormat());
	}

	/* 공백 기준 split */
	std::vector<std::string> tokens = split(start_line, ' ');
	if (tokens.size() != 3)
	{
		_logger << async::warning << __func__ << ": token count mismatch";
		throw(HTTP::InvalidFormat());
	}

	{
		_logger << async::verbose << __func__ << ": split into ";
		for (size_t i = 0; i < tokens.size(); i++)
			_logger << "\"" << tokens[i] << "\" ";
	}

	/* method index 구하기 */
	for (BidiMap<std::string, int>::const_iterator it = METHOD.begin();
		 it != METHOD.end(); it++)
	{
		if (tokens[0] == it->first)
		{
			_method = it->second;
			break;
		}
	}

	LOG_DEBUG(__func__ << ": method index is " << _method);
	if (_method == METHOD_NONE)
	{
		_logger << async::warning << __func__ << ": invalid method";
		throw(HTTP::InvalidValue());
	}

	/* uri, version 파싱 */
	_uri = tokens[1];
	size_t question_mark_pos = _uri.find('?');
	if (question_mark_pos != std::string::npos)
	{
		_query_string = getbackstr(_uri, question_mark_pos + 1);
		trimbackstr(_uri, question_mark_pos);
	}

	_version = tokens[2];

	{
		_logger << async::verbose << __func__ << ": URI: \"" << _uri << "\"";
		_logger << async::verbose << __func__ << ": QUERY: \"" << _query_string
				<< "\"";
		_logger << async::verbose << __func__ << ": version: \"" << _version
				<< "\"";
		LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");
	}
	return (RETURN_TYPE_OK);
}

int Request::consumeHeader(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		LOG_DEBUG(__func__ << ": buffer doesn't have CRLF");
		return (RETURN_TYPE_AGAIN);
	}

	const std::string header_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	LOG_DEBUG(__func__ << ": header line: " << header_line);
	LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		LOG_DEBUG(__func__ << ": header line only has CRLF (end of header)");
		LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");
		return (RETURN_TYPE_OK);
	}

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
	{
		_logger << async::warning << __func__ << ": header line has no colon";
		throw(HTTP::InvalidField());
	}

	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
	{
		_logger << async::warning << __func__ << ": header name has space";
		throw(HTTP::InvalidField());
	}

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, ",");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
	{
		strtrim(*it, LWS);
		LOG_DEBUG(__func__ << ": new value \"" << *it << "\"");
	}

	/* key가 있다면, 붙여넣기 */
	if (!_header.hasValue(name))
	{
		LOG_DEBUG(__func__ << ": assign to new header");
		_header.assign(name, values);
	}
	else
	{
		LOG_DEBUG(__func__ << ": insert to existing header");
		_header.insert(name, values);
	}

	return (RETURN_TYPE_IN_PROCESS);
}

int Request::consumeBody(std::string &buffer)
{
	if (buffer.size() < static_cast<size_t>(_content_length))
	{
		LOG_DEBUG(__func__ << ": not enough buffer size");
		return (RETURN_TYPE_AGAIN);
	}

	_body = consumestr(buffer, _content_length);
	LOG_DEBUG(__func__ << ": body result in :\"" << _body << "\"");
	LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");
	return (RETURN_TYPE_OK);
}

int Request::consumeChunk(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		LOG_DEBUG(__func__ << ": buffer doesn't have CRLF");
		return (RETURN_TYPE_AGAIN);
	}

	const size_t content_length = strtol(buffer.c_str(), NULL, 16);
	LOG_DEBUG(__func__ << ": content length " << content_length);
	if (content_length < 0)
	{
		_logger << async::warning << __func__
				<< ": negative content length of chunk";
		throw(HTTP::InvalidValue());
	}

	// buffer.size() >= 숫자가 적힌 줄 길이 + content_length + CRLF_LEN이면 ok
	if (buffer.size() < crlf_pos + CRLF_LEN + content_length + CRLF_LEN)
	{
		LOG_DEBUG(__func__ << ": not enough buffer size");
		return (RETURN_TYPE_AGAIN);
	}

	consumestr(buffer, crlf_pos + CRLF_LEN);
	_content_length += content_length;
	_body.append(consumestr(buffer, content_length));
	LOG_DEBUG(__func__ << ": body result in :\"" << _body << "\"");
	if (consumestr(buffer, CRLF_LEN) != CRLF)
	{
		_logger << async::warning << __func__ << ": chunk must end with CRLF";
		throw(HTTP::InvalidFormat());
	}
	LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");

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
		LOG_DEBUG(__func__ << ": buffer doesn't have CRLF");
		return (RETURN_TYPE_AGAIN);
	}

	const std::string header_line = consumestr(buffer, crlf_pos);
	consumestr(buffer, CRLF_LEN);
	LOG_DEBUG(__func__ << ": header line: " << header_line);
	LOG_DEBUG(__func__ << ": buffer result in :\"" << buffer << "\"");

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		LOG_DEBUG(__func__ << ": header line only has CRLF (end of header)");
		return (RETURN_TYPE_OK);
	}

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
	{
		_logger << async::warning << __func__ << ": header line has no colon";
		throw(HTTP::InvalidField());
	}

	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
	{
		_logger << async::warning << __func__ << ": header name has space";
		throw(HTTP::InvalidField());
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
		_logger << async::warning << __func__
				<< ": Trailer header doesn't have " << name;
		throw(HTTP::InvalidField());
	}

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, ",");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
	{
		strtrim(*it, LWS);
		LOG_DEBUG(__func__ << ": new value \"" << *it << "\"");
	}

	/** key가 있다면, 붙여넣기 **/
	if (!_header.hasValue(name))
	{
		_logger << async::verbose << __func__ << ": assign to new header";
		_header.assign(name, values);
	}
	else
	{
		_logger << async::verbose << __func__ << ": insert to existing header";
		_header.insert(name, values);
	}

	if (_trailer_values.size() == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}
