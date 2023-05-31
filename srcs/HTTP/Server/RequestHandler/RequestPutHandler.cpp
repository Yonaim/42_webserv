#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPutHandler::RequestPutHandler(Server *server,
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
		_logger << async::warning << "This location doesn't allow upload";
	}
}

Server::RequestPutHandler::~RequestPutHandler()
{
}

int Server::RequestPutHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	int rc = _writer.task();
	if (rc == async::status::OK_DONE)
	{
		std::string body = "made the file\n"
						   "click <A href=\""
						   + _resource_path + "\">here</A> to view it.";

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
		_logger << async::warning << _writer.errorMsg();
		registerErrorResponse(503); // Service Unavailable
	}
	else
	{
		_logger << async::warning << _writer.errorMsg();
		registerErrorResponse(500); // Internal Server Error
	}
	return (_status);
}
