#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location), _writer(NULL),
	  _cgi_handler(NULL)
{
	if (server->cgiEnabled() && server->isCGIextension(_resource_path))
	{
		// TODO: CGI 핸들러 완성시 주석 해제
		// _cgi_handler = new CGIHandler(args);
		return;
	}
	if (location.uploadAllowed())
	{
		_writer = new async::FileWriter(_server->_timeout_ms, _resource_path,
										request.getBody());
		return;
	}
	_response = _server->generateErrorResponse(500);
	_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	_logger << "This location doesn't allow uploads or CGI" << async::warning;
}

Server::RequestPostHandler::~RequestPostHandler()
{
	// TODO: CGI 핸들러 완성시 주석 해제
	// if (_cgi_handler)
	// 	delete _cgi_handler;
	if (_writer)
		delete _writer;
}

int Server::RequestPostHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (_cgi_handler)
	{
		CGI::Request cgi_request;
		setCGIRequestValues(cgi_request);
		// _status = _cgi_handler->task();
	}

	if (_writer)
	{
		try
		{
			int rc = _writer->task();
			if (rc == async::status::OK)
			{
				std::string body = "made the file\n"
								   "click <A href=\""
								   + _request.getURIPath()
								   + "\">here</A> to view it.";

				_response.setStatus(201); // Created
				_response.setLocation(_request.getURIPath());
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

	return (_status);
}
