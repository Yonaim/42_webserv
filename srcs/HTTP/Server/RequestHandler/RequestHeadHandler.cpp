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
	if (_status == RESPONSE_STATUS_OK || _status == RESPONSE_STATUS_ERROR)
		return (_status);

	if (isInvalidDirectoryFormat())
	{
		_response.setStatus(301);
		_response.setLocation(_request.getURIPath() + "/");
		_response.setContentLength(0);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		LOG_WARNING("invalid directory format, redirect to \""
					<< _request.getURIPath() + "\"");
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
		LOG_WARNING(_reader.errorMsg());
		setErrorCode(404); // Not Found
	}
	else if (rc == async::status::ERROR_FILEOPENING)
	{
		LOG_WARNING(_reader.errorMsg());
		setErrorCode(404); // Not Found
	}
	else
	{
		LOG_WARNING(_reader.errorMsg());
		setErrorCode(500); // Internal Server Error
	}
	return (_status);
}
