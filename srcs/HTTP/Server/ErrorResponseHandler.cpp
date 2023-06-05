#include "HTTP/Server.hpp"
#include "HTTP/const_values.hpp"
#include "HTTP/error_pages.hpp"

using namespace HTTP;

Server::ErrorResponseHandler::ErrorResponseHandler(
	Server *server, const int request_method, const int code,
	const unsigned int timeout_ms)
	: _request_method(request_method), _server(server),
	  _status(RESPONSE_STATUS_AGAIN), _code(code), _timeout_ms(timeout_ms),
	  _reader(NULL), _logger(async::Logger::getLogger("ErrorResponseHandler"))
{
	LOG_DEBUG("ErrorResponseHandler " << code << " timeout " << timeout_ms);
	if (_server->_error_page_paths.find(_code)
		== _server->_error_page_paths.end())
	{
		generateResponse(generateErrorPage(_code),
						 _request_method == METHOD_HEAD);
		_status = RESPONSE_STATUS_OK;
		return;
	}

	_reader = ft::shared_ptr<async::FileReader>(
		new async::FileReader(_timeout_ms, _server->_error_page_paths[_code]));
}

Server::ErrorResponseHandler::~ErrorResponseHandler()
{
}

void Server::ErrorResponseHandler::generateResponse(const std::string &body,
													bool is_head)
{
	_response.setStatus(_code);
	if (is_head)
		_response.setContentLength(0);
	else
	{
		_response.setBody(body);
		_response.setContentLength(body.length());
	}
	_response.setContentType("text/html");
}

int Server::ErrorResponseHandler::task(void)
{
	LOG_DEBUG("ErrorResponseHandler status " << _status);
	switch (_status)
	{
	case RESPONSE_STATUS_OK:
		break;

	case RESPONSE_STATUS_AGAIN:
		int rc = _reader->task();
		if (rc == async::status::OK_DONE)
		{
			generateResponse(_reader->retrieve(),
							 _request_method == METHOD_HEAD);
			_status = RESPONSE_STATUS_OK;
		}
		else if (rc == async::status::OK_AGAIN)
		{
			_status = RESPONSE_STATUS_AGAIN;
		}
		else if (rc == async::status::ERROR_TIMEOUT)
		{
			LOG_ERROR("Timeout while opening error page: "
					  + _reader->errorMsg());
			generateResponse(generateErrorPage(_code),
							 _request_method == METHOD_HEAD);
			_status = RESPONSE_STATUS_OK;
		}
		else
		{
			LOG_ERROR("Unknown error while loading error page for code "
					  << _code);
			std::string body = generateErrorPage(_code);
			generateResponse(generateErrorPage(_code),
							 _request_method == METHOD_HEAD);
			_status = RESPONSE_STATUS_OK;
		}
		break;
	}
	return (_status);
}

Response Server::ErrorResponseHandler::retrieve(void)
{
	if (_status != RESPONSE_STATUS_OK)
		throw(std::logic_error("Response has not been made yet."));
	return (_response);
}
