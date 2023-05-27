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

unsigned int Server::getTimeout(void) const
{
	return (_timeout_ms);
}

bool Server::cgiEnabled() const
{
	return (_cgi_enabled);
}

std::string Server::getErrorPage(const int code)
{
	if (_error_pages.find(code) == _error_pages.end())
		return (generateErrorPage(code));
	int rc = _error_pages[code]->task();
	if (rc == async::status::OK)
		return (_error_pages[code]->retrieve());
	else if (rc == async::status::AGAIN)
		return (generateErrorPage(code));
	// TODO: 예외 처리
	return ("");
}

Response Server::generateErrorResponse(const int code)
{
	Response response;

	std::string body = getErrorPage(code);
	response.setStatus(code);
	response.setBody(body);
	response.setContentLength(body.length());
	response.setContentType("text/html");
	return (response);
}

void Server::registerErrorResponse(const int fd, const int code)
{
	_output_queue[fd].push(generateErrorResponse(code));
}

void Server::registerRedirectResponse(const int fd,
									  const Server::Location &location)
{
	_output_queue[fd].push(location.generateRedirectResponse());
}

void Server::ensureClientConnected(int client_fd)
{
	if (_output_queue.find(client_fd) == _output_queue.end())
		throw(ClientNotFound(client_fd));
}
