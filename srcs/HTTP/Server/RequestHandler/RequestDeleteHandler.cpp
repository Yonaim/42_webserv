#include "HTTP/RequestHandler.hpp"
#include <errno.h>
#include <unistd.h>
#include <cstdio>

using namespace HTTP;

Server::RequestDeleteHandler::RequestDeleteHandler(
	Server *server, const Request &request, const Server::Location &location,
	const std::string &resource_path)
	: RequestHandler(server, request, location, resource_path)
{
}

Server::RequestDeleteHandler::~RequestDeleteHandler()
{
}

int Server::RequestDeleteHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (std::remove(_resource_path.c_str()) == -1)
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
