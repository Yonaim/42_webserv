#include "HTTP/RequestHandler.hpp"
#include <errno.h>
#include <unistd.h>

using namespace HTTP;

Server::RequestDeleteHandler::RequestDeleteHandler(
	Server *server, const Request &request, const Server::Location &location)
	: RequestHandler(server, request, location)
{
	if (server->cgiEnabled() && server->isCGIextension(_resource_path))
	{
		// TODO: CGI 핸들러 완성시 주석 해제
		// _cgi_handler = new CGIHandler(args);
		return;
	}
}

Server::RequestDeleteHandler::~RequestDeleteHandler()
{
}

int Server::RequestDeleteHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (_cgi_handler)
	{
		CGI::Request cgi_request;
		setCGIRequestValues(cgi_request);
		// _status = _cgi_handler->task();
	}

	if (unlink(_resource_path.c_str()) == -1)
	{
		switch (errno)
		{
		case ENOENT:
			// Not Found;
			_response = _server->generateErrorResponse(404);
			break;
		default:
			// Internal Server Error;
			_response = _server->generateErrorResponse(500);
			break;
		}
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	else
	{
		_response.setStatus(200);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	return (_status);
}
