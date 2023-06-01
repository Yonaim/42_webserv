#include "HTTP/Request.hpp"
#include "HTTP/const_values.hpp"
#include "utils/ansi_escape.h"
#include "utils/string.hpp"

using namespace HTTP;

static const char *consume_exc_description[]
	= {"Empty line found", "Invalid format", "Invalid header field",
	   "Invalid header value", "Invalid body size"};

static const char *parse_state_str[]
	= {"start line", "header", "body", "chunk", "trailer"};

Request::ParsingFail::ParsingFail(const std::string &why)
	: std::runtime_error(why)
{
}

Request::Request(void)
	: _method(METHOD_NONE), _current_state(PARSE_STATE_STARTLINE),
	  _content_length(0), _logger(async::Logger::getLogger("Request"))
{
}

Request::~Request()
{
}

Request::Request(const Request &orig)
	: _method(orig._method), _uri(orig._uri), _version(orig._version),
	  _header(orig._header), _body(orig._body),
	  _current_state(orig._current_state),
	  _content_length(orig._content_length), _logger(orig._logger)
{
}

Request &Request::operator=(const Request &orig)
{
	if (this != &orig)
	{
		_method = orig._method;
		_uri = orig._uri;
		_version = orig._version;
		_header = orig._header;
		_body = orig._body;
		_current_state = orig._current_state;
		_content_length = orig._content_length;
	}
	return (*this);
}

int Request::parse(std::string &buffer, size_t client_max_body_size)
{
	// TODO: 디버그 완료 후 하단 변수 삭제
	const char *state_names[]
		= {"STARTLINE", "HEADER", "BODY", "CHUNK", "TRAILER"};
	const char *code_names[] = {"OK", "INVALID", "AGAIN", "IN PROCESS"};

	while (true)
	{
		int rc;

		ASYNC_LOG_DEBUG(_logger, __func__ << ": current state is "
										  << state_names[_current_state]);
		switch (_current_state)
		{
		case PARSE_STATE_STARTLINE:
			rc = consumeStartLine(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << code_names[rc]);

			if (rc == RETURN_TYPE_OK)
				_current_state = PARSE_STATE_HEADER;
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			break;

		case PARSE_STATE_HEADER:
			rc = consumeHeader(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << code_names[rc]);

			if (rc == RETURN_TYPE_OK)
			{
				if (!_header.hasValue("Host")
					&& !_header.hasValue("Trailer", "Host"))
				{
					_logger << async::warning << __func__
							<< "Header must include Host header field";
					throwException(CONSUME_EXC_INVALID_VALUE);
				}
				if (_header.hasValue("Transfer-Encoding", "chunked"))
				{
					// Transfer coding == chunked라면
					_logger << async::verbose
							<< "header has Transfer-Encoding == chunked";
					if (_header.hasValue("Trailer"))
					{
						/** 유효한 Trailer 헤더 필드 값을 가지고 있는지 확인**/
						_trailer_values = _header.getValues("Trailer");
						if (_trailer_values.empty())
						{
							throwException(CONSUME_EXC_EMPTY_LINE);
						}
						std::vector<std::string>::const_iterator iter
							= _trailer_values.begin();
						for (; iter != _trailer_values.end(); ++iter)
						{
							if (*iter == "Trailer" || *iter == "Content-Length"
								|| *iter == "Transfer-Encoding")
								throwException(CONSUME_EXC_INVALID_FIELD);
						}
						_logger << async::verbose << "header has Trailer";
					}
					_current_state = PARSE_STATE_CHUNK;
				}
				else if (_header.hasValue("Trailer"))
				{
					_logger << async::warning << __func__
							<< ": Trailer with no Transfer-Encoding";
					throwException(CONSUME_EXC_INVALID_VALUE);
				}
				else if (_header.hasValue("Content-Length"))
				{
					if (_method == METHOD_GET || _method == METHOD_HEAD
						|| _method == METHOD_DELETE)
						throwException(CONSUME_EXC_INVALID_FIELD);
					const std::string &content_length
						= getHeaderValue("Content-Length", 0);
					_logger << async::verbose << "header has Content-Length";
					_logger << async::verbose
							<< "content-length : " << content_length;
					try
					{
						_content_length = toNum<size_t>(content_length);
					}
					catch (const std::exception &e)
					{
						throwException(CONSUME_EXC_INVALID_VALUE);
					}

					if (_content_length > client_max_body_size)
					{
						_logger << async::warning << __func__
								<< ": exceeds the client_max_body_size";
						throwException(CONSUME_EXC_INVALID_SIZE);
					}
					if (_content_length >= 0)
					{
						ASYNC_LOG_DEBUG(_logger,
										"header has Content-Length >= 0");
						_current_state = PARSE_STATE_BODY;
					}
				}
				else if (_method == METHOD_POST)
					throwException(CONSUME_EXC_INVALID_FIELD);
				else
					return (RETURN_TYPE_OK);
			}
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			else
				ASYNC_LOG_DEBUG(_logger, "parsing in progress...");
			break;

		case PARSE_STATE_BODY:
			rc = consumeBody(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << code_names[rc]);
			return (rc);

		case PARSE_STATE_CHUNK:
			rc = consumeChunk(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << code_names[rc]);
			ASYNC_LOG_DEBUG(_logger,
							"total content length " << _content_length);
			ASYNC_LOG_DEBUG(_logger,
							"client max body size " << client_max_body_size);
			if (_content_length > client_max_body_size)
				throwException(CONSUME_EXC_INVALID_SIZE);
			if (rc == RETURN_TYPE_OK)
			{
				if (_header.hasValue("Trailer"))
				{
					_logger << async::verbose << "header has Trailer";
					_current_state = PARSE_STATE_TRAILER;
				}
				else
					return (RETURN_TYPE_OK);
			}
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			else
				;
			break;

		case PARSE_STATE_TRAILER:
			rc = consumeTrailer(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << code_names[rc]);
			return (rc);
		}
	}
}

void Request::throwException(int code) const
{
	std::stringstream what;

	if (code < 0 || code > CONSUME_EXC_INVALID_SIZE)
		what << "Unknown error";
	else
		what << consume_exc_description[code];
	what << ": " << parse_state_str[_current_state];
	throw(ParsingFail(what.str()));
}

bool Request::hasHeaderValue(std::string const &name,
							 std::string const &value) const
{
	return (_header.hasValue(name, value));
}

bool Request::hasHeaderValue(const Header::const_iterator &name_iter,
							 const std::string &value) const
{
	return (_header.hasValue(name_iter, value));
}

bool Request::hasHeaderValue(std::string const &name) const
{
	return (_header.hasValue(name));
}

const std::string &Request::getHeaderValue(std::string const &name,
										   int idx) const
{
	return (_header.getValue(name, idx));
}

int Request::getMethod(void) const
{
	return (_method);
}

const std::string &Request::getMethodString(void) const
{
	return (HTTP::METHOD.getKeyByValue(_method));
}

const std::string &Request::getURIPath(void) const
{
	return (_uri);
}

const std::string &Request::getQueryString(void) const
{
	return (_query_string);
}

const std::string &Request::getBody() const
{
	return (_body);
}

const Header &Request::getHeader(void) const
{
	return (_header);
}

const std::string Request::getDescription(void) const
{
	const size_t bodylen = 20;

	std::stringstream buf;
	buf << ANSI_BBLUE;
	buf << "[" << HTTP::METHOD.getKeyByValue(_method) << " " << _uri << " | ";
	if (_body.size() > bodylen)
		buf << _body.substr(0, bodylen - 3) << "...";
	else
		buf << _body;
	buf << "]" << ANSI_RESET;
	return (buf.str());
}
