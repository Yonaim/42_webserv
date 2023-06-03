#include "HTTP/ParsingFail.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/const_values.hpp"
#include "utils/string.hpp"

using namespace HTTP;

void Request::parseHeaderEnsureHostHeaderField(void)
{
	if (!_header.hasValue("Host") && !_header.hasValue("Trailer", "Host"))
	{
		LOG_WARNING(__func__ << "Header must include Host header field");
		throw(HTTP::InvalidValue());
	}
}

void Request::parseHeaderEnsureTrailerHeaderField(void)
{
	if (!_header.hasValue("Transfer-Encoding", "chunked")
		&& _header.hasValue("Trailer"))
	{
		LOG_WARNING(__func__ << ": Trailer with no Transfer-Encoding");
		throw(HTTP::InvalidValue());
	}
}

void Request::parseHeaderEnsureCorrectHeadersForPostPutMethod(void)
{
	if ((_method == METHOD_POST || _method == METHOD_PUT)
		&& !_header.hasValue("Transfer-Encoding", "chunked")
		&& !_header.hasValue("Content-Length"))
		throw(HTTP::InvalidField());
}

void Request::parseHeaderHandleTransferEncodingChunked(void)
{
	if (_header.hasValue("Trailer"))
	{
		/** 유효한 Trailer 헤더 필드 값을 가지고 있는지 확인**/
		_trailer_values = _header.getValues("Trailer");
		if (_trailer_values.empty())
			throw(HTTP::EmptyLineFound());

		for (std::vector<std::string>::const_iterator iter
			 = _trailer_values.begin();
			 iter != _trailer_values.end(); ++iter)
		{
			if (*iter == "Trailer" || *iter == "Content-Length"
				|| *iter == "Transfer-Encoding")
				throw(HTTP::InvalidField());
		}
		LOG_VERBOSE("header has Trailer");
	}
	_current_state = PARSE_STATE_CHUNK;
}

void Request::parseHeaderHandlerContentLength(void)
{
	if (_method == METHOD_GET || _method == METHOD_HEAD
		|| _method == METHOD_DELETE)
		throw(HTTP::InvalidField());

	const std::string &content_length = getHeaderValue("Content-Length", 0);
	LOG_VERBOSE("header has Content-Length");
	LOG_VERBOSE("content-length : " << content_length);

	try
	{
		_content_length = toNum<size_t>(content_length);
	}
	catch (const std::invalid_argument &e)
	{
		throw(HTTP::InvalidValue());
	}
	_current_state = PARSE_STATE_BODY;
}
