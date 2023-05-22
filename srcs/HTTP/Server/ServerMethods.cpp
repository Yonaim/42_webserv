#include "../const_values.hpp"
#include "../error_pages.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

int Server::getPort(void) const
{
	return (_port);
}

std::string Server::getErrorPage(const int code)
{
	if (_error_pages.find(code) == _error_pages.end())
		return (generateErrorPage(code));
	int rc = _error_pages[code]->task();
	if (rc != async::status::OK)
		return (generateErrorPage(code));
	return (_error_pages[code]->retrieve());
}

Response Server::generateErrorResponse(const int code)
{
	Response response;

	std::string body = getErrorPage(code);
	response.setBody(body);
	response.setStatus(code);
	return (response);
}

void Server::registerErrorResponse(const int fd, const int code)
{
	_output_queue[fd].push(generateErrorResponse(code));
}

void Server::ensureClientConnected(int client_fd)
{
	if (_output_queue.find(client_fd) == _output_queue.end())
	{
		throw(std::runtime_error("Client fd " + toStr(client_fd)
								 + " is not yet connected."));
	}
}
