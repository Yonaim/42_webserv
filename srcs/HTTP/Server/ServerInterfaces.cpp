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
	for (std::map<int, std::queue<_RequestHandlerPtr> >::iterator it
		 = _request_handlers.begin();
		 it != _request_handlers.end(); it++)
	{
		int client_fd = it->first;
		std::queue<_RequestHandlerPtr> &handlers = it->second;
		if (handlers.empty())
			continue;
		int rc = handlers.front()->task();
		if (rc == RequestHandler::RESPONSE_STATUS_OK)
		{
			_output_queue[client_fd].push(handlers.front()->retrieve());
			handlers.pop();
			LOG_VERBOSE("Response for client " << client_fd
											   << " has been retrieved");
		}
		else if (rc == RequestHandler::RESPONSE_STATUS_AGAIN)
			continue;
		else
		{
			handlers.pop();
			LOG_ERROR("RequestHandler return code " << rc
													<< ", causing code 500");
			registerErrorResponse(client_fd, 500); // Internal Server Error}
		}
	}
}

void Server::iterateCGIHandlers(void)
{
	for (std::map<int, std::queue<_CGIRequestHandlerPtr> >::iterator it
		 = _cgi_handlers.begin();
		 it != _cgi_handlers.end(); it++)
	{
		int client_fd = it->first;
		std::queue<_CGIRequestHandlerPtr> &handlers = it->second;
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
				handlers.pop();
				LOG_VERBOSE("Response for client " << client_fd
												   << " has been retrieved");
			}
			else if (rc == CGI::RequestHandler::CGI_RESPONSE_STATUS_AGAIN)
				continue;
		}
		catch (std::exception &e)
		{
			handlers.pop();
			LOG_ERROR(e.what());
			LOG_ERROR("CGI failed, causing code 500");
			registerErrorResponse(client_fd, 500); // Internal Server Error
		}
	}
}

void Server::registerHTTPRequest(int client_fd, const Request &request,
								 const Server::Location &location,
								 const std::string &resource_path)
{
	const std::string &path = resource_path; // code shortener
	_RequestHandlerPtr handler;
	try
	{
		switch (request.getMethod())
		{
		case METHOD_GET:
			handler = _RequestHandlerPtr(
				new RequestGetHandler(this, request, location, path));
			break;
		case METHOD_HEAD:
			handler = _RequestHandlerPtr(
				new RequestHeadHandler(this, request, location, path));
			break;
		case METHOD_POST:
			handler = _RequestHandlerPtr(
				new RequestPostHandler(this, request, location, path));
			break;
		case METHOD_PUT:
			handler = _RequestHandlerPtr(
				new RequestPutHandler(this, request, location, path));
			break;
		case METHOD_DELETE:
			handler = _RequestHandlerPtr(
				new RequestDeleteHandler(this, request, location, path));
			break;
		default:
			registerErrorResponse(client_fd, 501); // Not Implemented
			return;
		}
	}
	catch (const LocationNotFound &e)
	{
		LOG_WARNING(e.what());
		registerErrorResponse(client_fd, 404); // Not Found
		return;
	}

	if (_request_handlers.find(client_fd) == _request_handlers.end())
		_request_handlers[client_fd] = std::queue<_RequestHandlerPtr>();
	if (_output_queue.find(client_fd) == _output_queue.end())
		_output_queue[client_fd] = std::queue<Response>();
	_request_handlers[client_fd].push(handler);
	LOG_VERBOSE("Registered HTTP RequestHandler for "
				<< METHOD[request.getMethod()]);
}

void Server::registerCGIRequest(int client_fd, const Request &request,
								const std::string &exec_path,
								const std::string &resource_path)
{
	CGI::Request cgi_request(request, resource_path);
	_CGIRequestHandlerPtr handler;
	try
	{
		if (request.getBody().size() > CGI::RequestHandler::pipeThreshold)
		{
			LOG_VERBOSE("Create CGI::RequestHandlerVnode for body size "
						<< request.getBody().size());
			handler = _CGIRequestHandlerPtr(new CGI::RequestHandlerVnode(
				cgi_request, exec_path, _timeout_ms, _temp_dir_path));
		}
		else
		{
			LOG_VERBOSE("Create CGI::RequestHandlerPipe for body size "
						<< request.getBody().size());
			handler = _CGIRequestHandlerPtr(new CGI::RequestHandlerPipe(
				cgi_request, exec_path, _timeout_ms));
		}
	}
	catch (const std::runtime_error &e)
	{
		LOG_WARNING(e.what());
		registerErrorResponse(client_fd, 500); // Internal Server Error
		return;
	}

	if (_cgi_handlers.find(client_fd) == _cgi_handlers.end())
		_cgi_handlers[client_fd] = std::queue<_CGIRequestHandlerPtr>();
	if (_output_queue.find(client_fd) == _output_queue.end())
		_output_queue[client_fd] = std::queue<Response>();
	_cgi_handlers[client_fd].push(handler);
	LOG_VERBOSE("Registered CGI RequestHandler for "
				<< METHOD[request.getMethod()]);
}

void Server::registerRequest(int client_fd, const Request &request)
{
	const Server::Location &location = getLocation(request.getURIPath());
	const std::string resource_path = location.generateResourcePath(request);
	const int method = request.getMethod();
	size_t location_body_size = location.getMaxBodySize();

	if (_http_min_version > request.getVersion()
		|| request.getVersion() > _http_max_version)
	{
		registerErrorResponse(client_fd, 505); // HTTP Version Not Supported
		return;
	}
	if (location_body_size != _max_body_size
		&& location_body_size < request.getBody().length())
	{
		registerErrorResponse(client_fd, 413);
		return;
	}
	if (cgiAllowed(method) && isCGIextension(request.getURIPath()))
	{
		const std::string &exec_path
			= _cgi_ext_to_path[getExtension(request.getURIPath())];
		registerCGIRequest(client_fd, request, exec_path, resource_path);
		return;
	}

	if (!location.isAllowedMethod(method))
	{
		LOG_INFO("Method " << METHOD[method] << " is not allowed");
		registerErrorResponse(client_fd, 405); // Method Not Allowed
		return;
	}
	if (location.doRedirect())
	{
		LOG_VERBOSE("Location " << location.getAlias()
								<< " redirect the request");
		registerRedirectResponse(client_fd, location);
		return;
	}

	registerHTTPRequest(client_fd, request, location, resource_path);
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
			_request_handlers[client_fd].pop();
	}
	if (_cgi_handlers.find(client_fd) != _cgi_handlers.end())
	{
		while (!_cgi_handlers[client_fd].empty())
			_cgi_handlers[client_fd].pop();
	}
	_request_handlers.erase(client_fd);
	_cgi_handlers.erase(client_fd);
	_output_queue.erase(client_fd);
	LOG_INFO("Disconnected client fd " << client_fd);
}
