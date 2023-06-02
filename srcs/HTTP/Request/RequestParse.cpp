#include "HTTP/ParsingFail.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/const_values.hpp"
#include "utils/string.hpp"

using namespace HTTP;

int Request::parse(std::string &buffer)
{
	while (true)
	{
		int rc;
		ASYNC_LOG_DEBUG(_logger,
						__func__ << ": current state is " << _current_state);
		switch (_current_state)
		{
		case PARSE_STATE_STARTLINE:
			rc = consumeStartLine(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << rc);

			if (rc == RETURN_TYPE_OK)
				_current_state = PARSE_STATE_HEADER;
			else if (rc == RETURN_TYPE_AGAIN)
				return (RETURN_TYPE_AGAIN);
			break;

		case PARSE_STATE_HEADER:
			rc = consumeHeader(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << rc);

			if (rc == RETURN_TYPE_OK)
			{
				if (!_header.hasValue("Host")
					&& !_header.hasValue("Trailer", "Host"))
				{
					_logger << async::warning << __func__
							<< "Header must include Host header field";
					throw(HTTP::InvalidValue());
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
							throw(HTTP::EmptyLineFound());
						}
						std::vector<std::string>::const_iterator iter
							= _trailer_values.begin();
						for (; iter != _trailer_values.end(); ++iter)
						{
							if (*iter == "Trailer" || *iter == "Content-Length"
								|| *iter == "Transfer-Encoding")
								throw(HTTP::InvalidField());
						}
						_logger << async::verbose << "header has Trailer";
					}
					_current_state = PARSE_STATE_CHUNK;
				}
				else if (_header.hasValue("Trailer"))
				{
					_logger << async::warning << __func__
							<< ": Trailer with no Transfer-Encoding";
					throw(HTTP::InvalidValue());
				}
				else if (_header.hasValue("Content-Length"))
				{
					if (_method == METHOD_GET || _method == METHOD_HEAD
						|| _method == METHOD_DELETE)
						throw(HTTP::InvalidField());
					const std::string &content_length
						= getHeaderValue("Content-Length", 0);
					_logger << async::verbose << "header has Content-Length";
					_logger << async::verbose
							<< "content-length : " << content_length;
					try
					{
						_content_length = toNum<size_t>(content_length);
					}
					catch (const std::invalid_argument &e)
					{
						throw(HTTP::InvalidValue());
					}

					if (_content_length > _max_body_size)
					{
						_logger << async::warning << __func__
								<< ": exceeds the _max_body_size";
						throw(HTTP::InvalidSize());
					}
					if (_content_length >= 0)
					{
						ASYNC_LOG_DEBUG(_logger,
										"header has Content-Length >= 0");
						_current_state = PARSE_STATE_BODY;
					}
				}
				else if (_method == METHOD_POST)
					throw(HTTP::InvalidField());
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
			ASYNC_LOG_DEBUG(_logger, "Got return code " << rc);
			return (rc);

		case PARSE_STATE_CHUNK:
			rc = consumeChunk(buffer);
			ASYNC_LOG_DEBUG(_logger, "Got return code " << rc);
			ASYNC_LOG_DEBUG(_logger,
							"total content length " << _content_length);
			ASYNC_LOG_DEBUG(_logger, "client max body size " << _max_body_size);
			if (_content_length > _max_body_size)
				throw(HTTP::InvalidSize());
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
			ASYNC_LOG_DEBUG(_logger, "Got return code " << rc);
			return (rc);
		}
	}
}
