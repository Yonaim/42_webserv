#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location), _writer(NULL)
{
	if (_cgi_handler)
		return;
	if (location.uploadAllowed())
	{
		_writer = new async::FileWriter(_server->_timeout_ms, _resource_path,
										request.getBody());
		return;
	}
	_response = _server->generateErrorResponse(500);
	_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	_logger << async::warning << "This location doesn't allow uploads or CGI";
}

Server::RequestPostHandler::~RequestPostHandler()
{
	if (_writer)
		delete _writer;
}

void Server::RequestPostHandler::handleRequest(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return;

	try
	{
		int rc = _writer->task();
		if (rc == async::status::OK)
		{
			std::string body = "made the file\n"
							   "click <A href=\""
							   + _resource_path + "\">here</A> to view it.";

			_response.setStatus(201); // Created
			_response.setLocation(_resource_path);
			_response.setBody(body);
			_response.setContentType("text/html");
			_response.setContentLength(body.length());
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
		else if (rc == async::status::AGAIN)
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
		}
		else
		{
			// TODO: 세분화된 예외 처리
			_response = _server->generateErrorResponse(500);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
	}
	catch (const async::FileIOProcessor::FileOpeningError &e)
	{
		registerErrorResponse(503, e); // Service Unavailable
	}
	catch (const std::exception &e)
	{
		registerErrorResponse(500, e); // Internal Server Error
	}
}
