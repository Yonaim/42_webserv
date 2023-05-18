#include "HTTPRequest.hpp"
#include "../const_values.hpp"
#include "../utils.hpp"
#include <iostream>

static const char *consume_exc_description[]
	= {"Empty line found", "Invalid format", "Invalid header field",
	   "Invalid header value"};

static const char *parse_state_str[]
	= {"start line", "header", "body", "chunk", "trailer", "CRLF after chunk"};

bool isSpace(const char c)
{
	return (c == ' ' || c == '\t');
}

HTTP::Request::Request(void)
	: _method(METHOD_NONE), _current_state(PARSE_STATE_STARTLINE)
{
}

HTTP::Request::~Request()
{
}

HTTP::Request::Request(const Request &orig)
	: _method(orig._method), _uri(orig._uri),
	  _version_string(orig._version_string), _version(orig._version),
	  _header(orig._header), _body(orig._body),
	  _current_state(orig._current_state)
{
}

HTTP::Request &HTTP::Request::operator=(const HTTP::Request &orig)
{
	if (this != &orig)
	{
		_method = orig._method;
		_uri = orig._uri;
		_version_string = orig._version_string;
		_version = orig._version;
		_header = orig._header;
		_body = orig._body;
		_current_state = orig._current_state;
	}
	return (*this);
}

int HTTP::Request::parse(std::string &buffer)
{
	// TODO: 디버그 완료 후 하단 변수 삭제
	const char *state_names[] = {"STARTLINE", "HEADER",  "BODY",
								 "CHUNK",     "TRAILER", "CRLF AFTER CHUNK"};
	const char *code_names[] = {"OK", "INVALID", "AGAIN", "IN PROCESS"};

	while (true)
	{
		int rc;

		std::cout << __func__ << ": current state is "
				  << state_names[_current_state] << std::endl;
		switch (_current_state)
		{
		case PARSE_STATE_STARTLINE:
			rc = consumeStartLine(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;

			if (rc == RETURN_TYPE_OK)
				_current_state = PARSE_STATE_HEADER;
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			break;

		case PARSE_STATE_HEADER:
			rc = consumeHeader(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;

			if (rc == RETURN_TYPE_OK)
			{
				if (!_header.hasValue("Host") && !_header.hasValue("Trailer","Host"))
				{
					_error_offset = 0;
					std::cout << __func__ << "Header must include Host header field" << std::endl;
					throwException(CONSUME_EXC_INVALID_VALUE);
				}
				if (_header.hasValue("Transfer-Encoding", "chunked"))
				{
					// Transfer coding == chunked라면
					std::cout << "header has Transfer-Encoding == chunked"
							  << std::endl;
					if (_header.hasValue("Trailer"))
					{
						/** 유효한 Trailer 헤더 필드 값을 가지고 있는지 확인**/
						_trailer_values = _header.getValues("Trailer");
						if (_trailer_values.empty())
						{
							_error_offset = 0;
							throwException(CONSUME_EXC_EMPTY_LINE);
						}
						std::vector<std::string>::const_iterator iter
							= _trailer_values.begin();
						for (; iter != _trailer_values.end(); ++iter)
						{
							if (*iter == "Trailer" || *iter == "Content-Length"
								|| *iter == "Transfer-Encoding")
							{
								_error_offset = 0;
								throwException(CONSUME_EXC_INVALID_FIELD);
							}
						}
						std::cout << "header has Trailer" << std::endl;
					}
					_current_state = PARSE_STATE_CHUNK;
				}
				else if (_header.hasValue("Trailer"))
				{
					_error_offset = 0;
					std::cout << __func__
							  << ": Trailer with no Transfer-Encoding"
							  << std::endl;
					throwException(CONSUME_EXC_INVALID_VALUE);
				}
				else if (_header.hasValue("Content-Length"))
				{
					// Content-length >= 0이라면
					const std::string &content_length
						= getHeaderValue("Content-Length", 0);
					std::cout << "header has Content-Length" << std::endl;

					std::cout << "content-length : " << content_length
							  << std::endl;
					_content_length = toNum<int>(content_length);
					if (_content_length >= 0)
					{
						std::cout << "header has Content-Length >= 0"
								  << std::endl;
						_current_state = PARSE_STATE_BODY;
					}
				}
				else
					return (RETURN_TYPE_OK);
			}
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			else
			{
				std::cout << "parsing ing..." << std::endl;
			}
			break;

		case PARSE_STATE_BODY:
			rc = consumeBody(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;
			return (rc);
			break;

		case PARSE_STATE_CHUNK:
			rc = consumeChunk(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;

			if (rc == RETURN_TYPE_OK)
			{
				if (_header.hasValue("Trailer"))
				{
					std::cout << "header has Trailer" << std::endl;
					_current_state = PARSE_STATE_TRAILER;
				}
				else
					_current_state = PARSE_STATE_CRLF_AFTER_CHUNK;
			}
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			else
				;
			break;

		case PARSE_STATE_TRAILER:
			rc = consumeTrailer(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;
			if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			else
				_current_state = PARSE_STATE_CRLF_AFTER_CHUNK;
			break;
		case PARSE_STATE_CRLF_AFTER_CHUNK:
			rc = consumeCRLF(buffer);
			std::cout << "Got return code " << code_names[rc] << std::endl;
			return (rc);
			break;
		}
	}
}

void HTTP::Request::throwException(int code) const
{
	std::stringstream what;

	if (code < 0 || code > CONSUME_EXC_INVALID_VALUE)
		what << "Unknown error";
	else
		what << consume_exc_description[code];
	what << ": " << parse_state_str[_current_state];
	throw(std::runtime_error(what.str()));
}

int HTTP::Request::consumeStartLine(std::string &buffer)
{
	// [메소드][SP][URI][SP][HTTP-version][CRLF]

	/* line 파싱 */
	const size_t crlf_pos = buffer.find(CRLF);

	if (crlf_pos == std::string::npos)
	{
		std::cout << __func__ << ": buffer doesn't have CRLF" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}
	if (crlf_pos == 0)
	{
		std::cout << __func__ << ": buffer's first line is empty" << std::endl;
		_error_offset = 0;
		throwException(CONSUME_EXC_EMPTY_LINE);
	}
	if (buffer[0] == ' ')
	{
		std::cout << __func__ << ": buffer's first character is space"
				  << std::endl;
		_error_offset = 0;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	const std::string start_line = getfrontstr(buffer, crlf_pos);
	std::cout << __func__ << ": start line is \"" << start_line << "\""
			  << std::endl;

	/* 공백 기준 split */
	std::vector<std::string> tokens = split(start_line, ' ');
	if (tokens.size() != 3)
	{
		std::cout << __func__ << ": token count mismatch" << std::endl;
		_error_offset = start_line.size();
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}

	std::cout << __func__ << ": split into ";
	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << "\"" << tokens[i] << "\" ";
	std::cout << std::endl;

	/* method index 구하기 */
	// TODO: const_values에 이미 저장되어있는 상수와 겹치는 상수는 삭제할 예정
	{
		if (tokens[0] == kMethodStr[i])
		{
			_method = i + 1;
			break;
		}
	}
	std::cout << __func__ << ": method index is " << _method << std::endl;
	if (_method == METHOD_NONE)
	{
		std::cout << __func__ << ": invalid method" << std::endl;
		_error_offset = tokens[0].size();
		throwException(CONSUME_EXC_INVALID_VALUE);
	}

	/* uri, version 파싱 */
	_uri = tokens[1];
	_version_string = tokens[2];
	std::cout << __func__ << ": URI: \"" << _uri << "\"" << std::endl;
	std::cout << __func__ << ": version: \"" << _version_string << "\""
			  << std::endl;

	trimfrontstr(buffer, start_line.size() + CRLF_LEN);

	std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
			  << std::endl;
	return (RETURN_TYPE_OK);
}

int HTTP::Request::consumeHeader(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		std::cout << __func__ << ": buffer doesn't have CRLF" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}

	if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	{
		std::cout << __func__ << ": header line only has CRLF (end of header)"
				  << std::endl;
		trimfrontstr(buffer, CRLF_LEN);
		std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
				  << std::endl;
		return (RETURN_TYPE_OK);
	}

	const std::string header_line = getfrontstr(buffer, crlf_pos);
	std::cout << __func__ << ": header line: " << header_line << std::endl;

	/* name 파싱 */
	size_t key_end_idx = 0;
	std::string name = strBeforeSep(header_line, ":", key_end_idx);
	if (name == "" || hasSpace(name))
	{
		std::cout << __func__ << ": header has invalid name" << std::endl;
		throwException(CONSUME_EXC_INVALID_FIELD);
	}
	std::cout << __func__ << ": header name is " << name << std::endl;

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
		std::cout << __func__ << ": push value " << vec_values.back()
				  << std::endl;
	}
	std::string value = strtrim(std::string(&header_line[key_end_idx]), " ");
	if (value != "")
	{
		vec_values.push_back(value);
		std::cout << __func__ << ": push value " << vec_values.back()
				  << std::endl;
	}

	/* key가 있다면, 붙여넣기 */
	if (!_header.hasValue(name))
	{
		std::cout << __func__ << ": assign to new header" << std::endl;
		_header.assign(name, vec_values);
	}
	else
	{
		std::cout << __func__ << ": insert to existing header" << std::endl;
		_header.insert(name, vec_values);
	}

	trimfrontstr(buffer, header_line.size() + CRLF_LEN);
	std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
			  << std::endl;

	return (RETURN_TYPE_IN_PROCESS);
}

int HTTP::Request::consumeBody(std::string &buffer)
{
	// size_t content_length = 0; // TODO: 헤더 맵에서 Content-length 파싱해오기

	if (buffer.size() < static_cast<size_t>(_content_length))
	{
		std::cout << __func__ << ": not enough buffer size" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}

	_body = getfrontstr(buffer, _content_length);
	std::cout << __func__ << ": body result in :\"" << _body << "\""
			  << std::endl;
	trimfrontstr(buffer, _content_length);
	std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
			  << std::endl;
	return (RETURN_TYPE_OK);
}

int HTTP::Request::consumeChunk(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		std::cout << __func__ << ": buffer doesn't have CRLF" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}
	const int content_length = strtol(buffer.c_str(), NULL, 16);
	std::cout << __func__ << ": content length " << content_length << std::endl;
	// TODO: content length 0일때 마지막 청크 처리
	if (content_length < 0)
	{
		std::cout << __func__ << ": negative content length of chunk"
				  << std::endl;
		throwException(CONSUME_EXC_INVALID_VALUE);
	}
	// buffer.size() >= 숫자 길이 + content_length + CRLF_LEN * 2이면 ok
	if (buffer.size() < crlf_pos + content_length + CRLF_LEN * 2)
	{
		return (RETURN_TYPE_AGAIN);
	}
	_body.append(buffer.substr(crlf_pos + CRLF_LEN, content_length));
	std::cout << __func__ << ": body result in :\"" << _body << "\""
			  << std::endl;
	if (buffer.substr(crlf_pos + CRLF_LEN + content_length, CRLF_LEN) != CRLF)
	{
		std::cout << __func__ << ": chunk must end with CRLF" << std::endl;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	trimfrontstr(buffer, crlf_pos + CRLF_LEN * 2 + content_length);
	std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
			  << std::endl;

	if (content_length == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}

int HTTP::Request::consumeTrailer(std::string &buffer)
{
	/**  헤더라인 파싱하기  **/
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		std::cout << __func__ << ": buffer doesn't have CRLF" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}
	const std::string header_line = getfrontstr(buffer, crlf_pos);
	std::cout << __func__ << ": header line: " << header_line << std::endl;
	// Trailer는 헤더와 달리 끝에 CRLF가 하나 더 붙지 않아서 아래 코드
	// 주석처리함 if (crlf_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
	// {
	// 	trimfrontstr(buffer, CRLF_LEN);
	// 	return (RETURN_TYPE_OK);
	// }

	/** name 파싱 **/
	size_t key_end_idx = 0;
	const std::string name = strBeforeSep(header_line, ":", key_end_idx);
	if (name == "" || hasSpace(name))
	{
		std::cout << __func__ << ": header has invalid name" << std::endl;
		_error_offset = key_end_idx;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	std::cout << __func__ << ": header name is " << name << std::endl;

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
		std::cout << __func__ << ": Trailer header doesn't have " << name
				  << std::endl;
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
		std::cout << __func__ << ": push value " << vec_values.back()
				  << std::endl;
	}
	std::string value = strtrim(std::string(&header_line[key_end_idx]), " ");
	if (value != "")
	{
		vec_values.push_back(value);
		std::cout << __func__ << ": push value " << vec_values.back()
				  << std::endl;
	}

	/** key가 있다면, 붙여넣기 **/
	if (!_header.hasValue(name))
	{
		std::cout << __func__ << ": assign to new header" << std::endl;
		_header.assign(name, vec_values);
	}
	else
	{
		std::cout << __func__ << ": insert to existing header" << std::endl;
		_header.insert(name, vec_values);
	}

	trimfrontstr(buffer, header_line.size() + CRLF_LEN);
	std::cout << __func__ << ": buffer result in :\"" << buffer << "\""
			  << std::endl;

	if (_trailer_values.size() == 0)
		return (RETURN_TYPE_OK);
	else
		return (RETURN_TYPE_IN_PROCESS);
}

int HTTP::Request::consumeCRLF(std::string &buffer)
{
	const size_t crlf_pos = buffer.find(CRLF);
	if (crlf_pos == std::string::npos)
	{
		std::cout << __func__ << ": buffer doesn't have CRLF" << std::endl;
		return (RETURN_TYPE_AGAIN);
	}
	else if (crlf_pos == 0)
	{
		trimfrontstr(buffer, CRLF_LEN);
		return (RETURN_TYPE_OK);
	}
	else
	{
		_error_offset = 0;
		throwException(CONSUME_EXC_INVALID_FORMAT);
	}
	/* unreachable */
	return (RETURN_TYPE_INVALID);
}

bool HTTP::Request::hasHeaderValue(std::string const &name,
								   std::string const &value) const
{
	return (_header.hasValue(name, value));
}

bool HTTP::Request::hasHeaderValue(const Header::const_iterator &name_iter,
								   const std::string &value) const
{
	return (_header.hasValue(name_iter, value));
}

bool HTTP::Request::hasHeaderValue(std::string const &name) const
{
	return (_header.hasValue(name));
}

const std::string &HTTP::Request::getHeaderValue(std::string const &name,
												 int idx) const
{
	return (_header.getValue(name, idx));
}
