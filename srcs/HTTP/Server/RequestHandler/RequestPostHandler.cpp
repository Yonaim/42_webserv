#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location),
	  _writer(1000, _resource_path, request.getBody())
{
}

Server::RequestPostHandler::~RequestPostHandler()
{
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
