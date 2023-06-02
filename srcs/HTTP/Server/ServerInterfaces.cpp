#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/const_values.hpp"
#include "HTTP/error_pages.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

void Server::task(void)
{
	iterateRequestHandlers();
	iterateCGIHandlers();
}

void Server::iterateRequestHandlers(void)
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

void Server::iterateCGIHandlers(void)
{
	for (std::map<int, std::queue<CGI::RequestHandler *> >::iterator it
		 = _cgi_handlers.begin();
		 it != _cgi_handlers.end(); it++)
	{
		int client_fd = it->first;
		std::queue<CGI::RequestHandler *> &handlers = it->second;
		if (handlers.empty())
			continue;

		try
		{
			int rc = handlers.front()->task();
			if (rc == CGI::RequestHandler::CGI_RESPONSE_STATUS_OK)
			{
				const CGI::Response &cgi_response
					= handlers.front()->retrieve();
				_output_queue[client_fd].push(cgi_response.toHTTPResponse());
				delete handlers.front();
				handlers.pop();
				_logger << async::verbose << "Response for client " << client_fd
						<< " has been retrieved";
			}
			else if (rc == CGI::RequestHandler::CGI_RESPONSE_STATUS_AGAIN)
				continue;
		}
		catch (std::exception &e)
		{
			delete handlers.front();
			handlers.pop();
			_logger << async::error << e.what();
			_logger << async::error << "CGI failed, causing code 500";
			registerErrorResponse(client_fd, 500); // Internal Server Error
		}
	}
}

void Server::registerRequest(int client_fd, const Request &request)
{
	const Server::Location &location = getLocation(request.getURIPath());
	const std::string resource_path = location.generateResourcePath(request);
	const int method = request.getMethod();
	size_t location_body_size = location.getMaxBodySize();

	if (location_body_size != _max_body_size
		&& location_body_size < request.getBody().length())
	{
		registerErrorResponse(client_fd, 413);
		return;
	}
	if (cgiAllowed(method) && isCGIextension(request.getURIPath()))
	{
		registerCGIRequest(client_fd, request, resource_path);
		return;
	}

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

	registerHTTPRequest(client_fd, request, location, resource_path);
}

void Server::registerHTTPRequest(int client_fd, const Request &request,
								 const Server::Location &location,
								 const std::string &resource_path)
{
	const std::string &path = resource_path; // code shortener
	RequestHandler *handler;
	try
	{
		switch (request.getMethod())
		{
		case METHOD_GET:
			handler = new RequestGetHandler(this, request, location, path);
			break;
		case METHOD_HEAD:
			handler = new RequestHeadHandler(this, request, location, path);
			break;
		case METHOD_POST:
			handler = new RequestPostHandler(this, request, location, path);
			break;
		case METHOD_PUT:
			handler = new RequestPutHandler(this, request, location, path);
			break;
		case METHOD_DELETE:
			handler = new RequestDeleteHandler(this, request, location, path);
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
		return;
	}

	if (_request_handlers.find(client_fd) == _request_handlers.end())
		_request_handlers[client_fd] = std::queue<RequestHandler *>();
	if (_output_queue.find(client_fd) == _output_queue.end())
		_output_queue[client_fd] = std::queue<Response>();
	_request_handlers[client_fd].push(handler);
	_logger << async::verbose << "Registered HTTP RequestHandler for "
			<< METHOD[request.getMethod()];
}

void Server::registerCGIRequest(int client_fd, const Request &request,
								const std::string &resource_path)
{
	CGI::Request cgi_request(request, resource_path);
	CGI::RequestHandler *handler;
	try
	{
		if (request.getBody().size() > CGI::RequestHandler::pipeThreshold)
		{
			_logger << async::verbose
					<< "Create CGI::RequestHandlerVnode for body size "
					<< request.getBody().size();
			handler = new CGI::RequestHandlerVnode(cgi_request, _cgi_exec_path,
												   _timeout_ms, _temp_dir_path);
		}
		else
		{
			_logger << async::verbose
					<< "Create CGI::RequestHandlerPipe for body size "
					<< request.getBody().size();
			handler = new CGI::RequestHandlerPipe(cgi_request, _cgi_exec_path,
												  _timeout_ms);
		}
	}
	catch (const std::runtime_error &e)
	{
		_logger << async::warning << e.what();
		registerErrorResponse(client_fd, 500); // Internal Server Error
		return;
	}

	if (_cgi_handlers.find(client_fd) == _cgi_handlers.end())
		_cgi_handlers[client_fd] = std::queue<CGI::RequestHandler *>();
	if (_output_queue.find(client_fd) == _output_queue.end())
		_output_queue[client_fd] = std::queue<Response>();
	_cgi_handlers[client_fd].push(handler);
	_logger << async::verbose << "Registered CGI RequestHandler for "
			<< METHOD[request.getMethod()];
}

Response Server::retrieveResponse(int client_fd)
{
	ensureClientConnected(client_fd);
	if (_output_queue[client_fd].empty())
		throw(std::logic_error("No response made for client fd "
							   + toStr(client_fd)));
	Response res = _output_queue[client_fd].front();
	_output_queue[client_fd].pop();
	return (res);
}

void Server::registerRedirectResponse(int fd, const Server::Location &location)
{
	_output_queue[fd].push(location.generateRedirectResponse());
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

void Server::registerErrorResponse(int fd, int code)
{
	_output_queue[fd].push(generateErrorResponse(code));
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
	if (_cgi_handlers.find(client_fd) != _cgi_handlers.end())
	{
		while (!_cgi_handlers[client_fd].empty())
		{
			delete _cgi_handlers[client_fd].front();
			_cgi_handlers[client_fd].pop();
		}
	}
	_request_handlers.erase(client_fd);
	_cgi_handlers.erase(client_fd);
	_output_queue.erase(client_fd);
	_logger << async::info << "Disconnected client fd " << client_fd;
}
