#include "HTTP/RequestHandler.hpp"
#include <unistd.h>

using namespace HTTP;

Server::RequestDeleteHandler::RequestDeleteHandler(Server *server,
												   const Request &request)
	: RequestHandler(server, request)
{
	//
}

Server::RequestDeleteHandler::~RequestDeleteHandler()
{
}

// 호출 금지
Server::RequestDeleteHandler::RequestDeleteHandler(
	const RequestDeleteHandler &orig)
	: Server::RequestHandler(orig._server, orig._request)
{
}

// 호출 금지
Server::RequestDeleteHandler &Server::RequestDeleteHandler::operator=(
	const RequestDeleteHandler &orig)
{
	(void)orig;
	return (*this);
}

int Server::RequestDeleteHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (unlink(_resource_path.c_str()) == -1)
	{
		_response.setStatus(404);
		// _response.setBody(_error_pages.find(404)->second);
		_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
	}
	else
	{
		_response.setStatus(200);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	return (_status);
}
