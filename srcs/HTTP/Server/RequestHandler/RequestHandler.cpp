#include "HTTP/RequestHandler.hpp"
#include "utils/file.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::RequestHandler::RequestHandler(Server *server, const Request &request,
									   const Server::Location &location)
	: _request(request), _location(location), _server(server),
	  _status(RESPONSE_STATUS_AGAIN),
	  _resource_path(server->getResourcePath(request)),
	  _logger(async::Logger::getLogger("RequestHandler")), _cgi_handler(NULL)
{
	if (_request.hasHeaderValue("Connection", "close"))
		_response.setConnection(false);
	else
		_response.setConnection(true);
}

Server::RequestHandler::~RequestHandler()
{
}

void Server::RequestHandler::registerErrorResponse(const int code,
												   const std::exception &e)
{
	_response = _server->generateErrorResponse(code);
	_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	_server->_logger << async::warning << e.what();
}

Response Server::RequestHandler::retrieve(void)
{
	if (_status != RESPONSE_STATUS_OK)
		throw(std::runtime_error("Response has not been made yet."));
	return (_response);
}

void Server::RequestHandler::setCGIRequestValues(CGI::Request &cgi_request)
{
	// TODO: setCGIRequestValues 완성하기
	cgi_request.setMetaVariable("CONTENT_LENGTH",
								toStr<size_t>(_request.getBody().length()));
	if (_request.hasHeaderValue("Content-Type"))
		cgi_request.setMetaVariable("CONTENT_TYPE",
									_request.getHeaderValue("Content-Type", 0));
	cgi_request.setMetaVariable("PATH_INFO", _request.getURIPath());
	cgi_request.setMetaVariable("QUERY_STRING", _request.getQueryString());
	cgi_request.setMetaVariable("REQUEST_METHOD", _request.getMethodString());
	std::string host_header = _request.getHeaderValue("Host", 0);
	size_t colon_pos = host_header.find(':');
	if (colon_pos == std::string::npos)
	{
		cgi_request.setMetaVariable("SERVER_NAME", host_header);
		cgi_request.setMetaVariable("SERVER_PORT", "80");
	}
	else
	{
		cgi_request.setMetaVariable("SERVER_NAME",
									getfrontstr(host_header, colon_pos));
		cgi_request.setMetaVariable("SERVER_PORT",
									getbackstr(host_header, colon_pos + 1));
	}
}

void Server::RequestHandler::CGIResponseToHTTPResponse(
	const CGI::Response &cgi_response)
{
	// TODO: 구현
	(void)cgi_response;
}

void Server::RequestHandler::handleCGI(void)
{
	try
	{
		CGI::Request cgi_request;
		setCGIRequestValues(cgi_request);
		CGI::RequestHandler cgi_request_handler(cgi_request, 1000);

		int rc = cgi_request_handler.task();
		if (rc == CGI::RequestHandler::CGI_RESPONSE_STATUS_OK)
		{
			const CGI::Response &cgi_response = cgi_request_handler.retrieve();
			CGIResponseToHTTPResponse(cgi_response);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
		else
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
		}
	}
	catch (const std::exception &e)
	{
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		registerErrorResponse(500, e); // Internal Server Error
	}
}

bool Server::RequestHandler::isDirectory(void) const
{
	return (::isDirectory(_resource_path));
}

bool Server::RequestHandler::isInvalidDirectoryFormat(void) const
{
	if (_resource_path == "/")
		return (false);
	if (isDirectory() && _resource_path.back() != '/')
		return (true);
	return (false);
}
