#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request)
	: RequestHandler(server, request),
	  _writer(1000, _resource_path, request.getBody())
{
}

Server::RequestPostHandler::~RequestPostHandler()
{
}

// 호출 금지
Server::RequestPostHandler::RequestPostHandler(const RequestPostHandler &orig)
	: Server::RequestHandler(orig._server, orig._request), _writer(0, 0, 0)
{
}

// 호출 금지
Server::RequestPostHandler &Server::RequestPostHandler::operator=(
	const RequestPostHandler &orig)
{
	(void)orig;
	return (*this);
}

int Server::RequestPostHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	try
	{
		int rc = _writer.task();
		if (rc == async::status::OK)
		{
			_response.setStatus(201); // Created
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
		else if (rc == async::status::AGAIN)
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
		}
		else
		{
			// TODO: 예외 처리
			_response.setStatus(404);
			// _response.setBody(_error_pages.find(404)->second);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
	}
	catch (const async::FileIOProcessor::FileOpeningError &e)
	{
		// Service Unavailable
		_response = _server->generateErrorResponse(503);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		_server->_logger << e.what() << async::warning;
	}
	catch (const std::exception &e)
	{
		// Internal Server Error
		_response = _server->generateErrorResponse(500);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		_server->_logger << e.what() << async::warning;
	}

	return (_status);
}
