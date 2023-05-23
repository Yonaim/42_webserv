#include "HTTP/RequestHandler.hpp"

using namespace HTTP;

Server::RequestHeadHandler::RequestHeadHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location), _reader(1000, _resource_path)
{
}

Server::RequestHeadHandler::~RequestHeadHandler()
{
}

int Server::RequestHeadHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	try
	{
		int rc = _reader.task();
		if (rc == async::status::OK)
		{
			const std::string &content = _reader.retrieve();
			_response.setStatus(200);
			_response.setContentLength(content.length());
			_response.setContentType(_resource_path);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
		else if (rc == async::status::AGAIN)
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
		}
		else
		{
			// TODO: 세분화된 예외 처리
			_response = _server->generateErrorResponse(500);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
	}
	catch (const async::IOProcessor::FileIsDirectory &e)
	{
		registerErrorResponse(404, e); // Not Found
	}
	catch (const async::FileIOProcessor::FileOpeningError &e)
	{
		registerErrorResponse(404, e); // Not Found
	}
	catch (const std::exception &e)
	{
		// Internal Server Error
		registerErrorResponse(500, e); // Not Found
	}
	return (_status);
}
