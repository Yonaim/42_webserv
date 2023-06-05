#include "HTTP/RequestHandler.hpp"
#include <cerrno>
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
	if (_status == RESPONSE_STATUS_OK || _status == RESPONSE_STATUS_ERROR)
		return (_status);

	if (std::remove(_resource_path.c_str()) == -1)
	{
		switch (errno)
		{
		case ENOENT:
			// Not Found;
			setErrorCode(404);
			break;
		default:
			// Internal Server Error;
			setErrorCode(500);
			break;
		}
	}
	else
	{
		_response.setStatus(200);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	return (_status);
}
