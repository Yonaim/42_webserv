#include "HTTP/RequestHandler.hpp"
#include "async/FileIOHandler.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request,
											   const Server::Location &location,
											   const std::string &resource_path)
	: RequestHandler(server, request, location, resource_path),
	  _writer(_server->_timeout_ms, _resource_path, request.getBody())
{
	if (!location.uploadAllowed())
	{
		_response = _server->generateErrorResponse(500);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		LOG_WARNING("This location doesn't allow upload");
	}
}

Server::RequestPostHandler::~RequestPostHandler()
{
}

int Server::RequestPostHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	int rc = _writer.task();
	if (rc == async::status::OK_DONE)
	{
		std::string body = "made the file\n"
						   "click <A href=\""
						   + _resource_path + "\">here</A> to view it.\n";

		_response.setStatus(201); // Created
		_response.setLocation(_resource_path);
		_response.setBody(body);
		_response.setContentType("text/html");
		_response.setContentLength(body.length());
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	else if (rc == async::status::OK_AGAIN)
	{
		_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
	}
	else if (rc == async::status::ERROR_FILEOPENING)
	{
		LOG_WARNING(_writer.errorMsg());
		registerErrorResponse(503); // Service Unavailable
	}
	else
	{
		LOG_WARNING(_writer.errorMsg());
		registerErrorResponse(500); // Internal Server Error
	}
	return (_status);
}
