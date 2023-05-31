#include "HTTP/RequestHandler.hpp"

using namespace HTTP;

Server::RequestHeadHandler::RequestHeadHandler(Server *server,
											   const Request &request,
											   const Server::Location &location,
											   const std::string &resource_path)
	: RequestHandler(server, request, location, resource_path),
	  _reader(_server->_timeout_ms, _resource_path)
{
}

Server::RequestHeadHandler::~RequestHeadHandler()
{
}

int Server::RequestHeadHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (isInvalidDirectoryFormat())
	{
		_response = _server->generateErrorResponse(301); // Not Found;
		_response.setValue("Location", _request.getURIPath() + "/");
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		_logger << async::warning << "invalid directory format, redirect to \""
				<< _request.getURIPath() + "\"";
		return (_status);
	}

	int rc = _reader.task();
	if (rc == async::status::OK_DONE)
	{
		const std::string &content = _reader.retrieve();
		_response.setStatus(200);
		_response.setContentLength(content.length());
		_response.setContentType(_resource_path);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	else if (rc == async::status::OK_AGAIN)
	{
		_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
	}
	else if (rc == async::status::ERROR_FILEISDIR)
	{
		_logger << async::warning << _reader.errorMsg();
		registerErrorResponse(404); // Not Found
	}
	else if (rc == async::status::ERROR_FILEOPENING)
	{
		_logger << async::warning << _reader.errorMsg();
		registerErrorResponse(404); // Not Found
	}
	else
	{
		_logger << async::warning << _reader.errorMsg();
		registerErrorResponse(500); // Internal Server Error
	}
	return (_status);
}
