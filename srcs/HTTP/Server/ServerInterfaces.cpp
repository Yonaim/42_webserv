#include "HTTP/const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/error_pages.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

void Server::task(void)
{
	for (std::map<int, std::queue<RequestHandler *> >::iterator it
		 = _request_handlers.begin();
		 it != _request_handlers.end(); it++)
	{
		int client_fd = it->first;
		std::queue<RequestHandler *> &handlers = it->second;
		if (handlers.empty())
			continue;
		int rc = handlers.front()->task();
		if (rc == RequestHandler::RESPONSE_STATUS_OK)
		{
			_output_queue[client_fd].push(handlers.front()->retrieve());
			delete handlers.front();
			handlers.pop();
			_logger << async::verbose << "Response for client " << client_fd
					<< " has been retrieved";
		}
		else if (rc == RequestHandler::RESPONSE_STATUS_AGAIN)
			continue;
		else
		{
			delete handlers.front();
			handlers.pop();
			_logger << async::error << "RequestHandler return code " << rc
					<< ", causing code 500";
			registerErrorResponse(client_fd, 500); // Internal Server Error}
		}
	}
}

bool Server::isForMe(const Request &request)
{
	if (!request.hasHeaderValue("Host"))
		throw(std::runtime_error(
			"HTTP Request has no header field with name \"Host\""));
	// TODO: Host 헤더 필드가 여러개일 때 예외 처리
	const std::string &host = request.getHeaderValue("Host", 0);
	_logger << async::verbose << "Request is for host \"" << host << "\"";
	for (std::set<std::string>::iterator it = _server_name.begin();
		 it != _server_name.end(); it++)
	{
		if (*it == host)
		{
			_logger << async::verbose << "Request host match with \"" << host
					<< "\"";
			return (true);
		}
	}
	return (false);
}

void Server::registerRequest(int client_fd, const Request &request)
{
	const Server::Location &location = getLocation(request.getURIPath());
	const int method = request.getMethod();
	if (!location.isAllowedMethod(method))
	{
		_logger << async::info << "Method " << METHOD[method]
				<< " is not allowed";
		registerErrorResponse(client_fd, 405); // Method Not Allowed
		return;
	}
	if (location.doRedirect())
	{
		_logger << async::verbose << "Location " << location.getAlias()
				<< " redirect the request";
		registerRedirectResponse(client_fd, location);
		return;
	}

	RequestHandler *handler;
	try
	{
		switch (method)
		{
		case METHOD_GET:
			handler = new RequestGetHandler(this, request, location);
			break;
		case METHOD_HEAD:
			handler = new RequestHeadHandler(this, request, location);
			break;
		case METHOD_POST:
			handler = new RequestPostHandler(this, request, location);
			break;
		case METHOD_DELETE:
			handler = new RequestDeleteHandler(this, request, location);
			break;
		default:
			registerErrorResponse(client_fd, 501); // Not Implemented
			return;
		}
	}
	catch (const LocationNotFound &e)
	{
		_logger << async::warning << e.what();
		registerErrorResponse(client_fd, 404); // Not Found
	}

	if (_request_handlers.find(client_fd) == _request_handlers.end())
	{
		_request_handlers[client_fd] = std::queue<RequestHandler *>();
		_output_queue[client_fd] = std::queue<Response>();
	}
	_request_handlers[client_fd].push(handler);
	_logger << async::verbose << "Registered RequestHandler for "
			<< METHOD[request.getMethod()];
}

Response Server::retrieveResponse(int client_fd)
{
	ensureClientConnected(client_fd);
	if (_output_queue[client_fd].empty())
		throw(std::runtime_error("No response made for client fd "
								 + toStr(client_fd)));
	Response res = _output_queue[client_fd].front();
	_output_queue[client_fd].pop();
	return (res);
}

int Server::hasResponses(void)
{
	for (std::map<int, std::queue<Response> >::iterator it
		 = _output_queue.begin();
		 it != _output_queue.end(); it++)
	{
		if (!it->second.empty())
			return (it->first);
	}
	return (-1);
}

bool Server::hasResponses(int client_fd)
{
	ensureClientConnected(client_fd);
	return (!_output_queue[client_fd].empty());
}

bool Server::hasServerName(void) const
{
	return (_has_server_name);
}

void Server::disconnect(int client_fd)
{
	ensureClientConnected(client_fd);
	if (_request_handlers.find(client_fd) != _request_handlers.end())
	{
		while (!_request_handlers[client_fd].empty())
		{
			delete _request_handlers[client_fd].front();
			_request_handlers[client_fd].pop();
		}
	}
	_request_handlers.erase(client_fd);
	_output_queue.erase(client_fd);
	_logger << async::info << "Disconnected client fd " << client_fd;
}
