#include "HTTP/RequestHandler.hpp"

using namespace HTTP;

Server::RequestHandler::RequestHandler(void)
{
}

Server::RequestHandler::RequestHandler(Server *server, const Request &request)
	: _request(request), _status(RESPONSE_STATUS_AGAIN), _server(server),
	  _resource_path(server->getResourcePath(request))
{
	if (_request.hasHeaderValue("Connection", "close"))
		_response.setConnection(false);
	else
		_response.setConnection(true);
}

Server::RequestHandler::~RequestHandler()
{
}

Server::RequestHandler::RequestHandler(const RequestHandler &orig)
	: _request(orig._request), _response(orig._response), _status(orig._status),
	  _server(orig._server)
{
}

Server::RequestHandler &Server::RequestHandler::operator=(
	const RequestHandler &orig)
{
	(void)orig;
	return (*this);
}

Response Server::RequestHandler::retrieve(void)
{
	if (_status != RESPONSE_STATUS_OK)
		throw(std::runtime_error("Response has not been made yet."));
	return (_response);
}
