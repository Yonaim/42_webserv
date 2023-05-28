#include "HTTP/RequestHandler.hpp"

using namespace HTTP;

Server::RequestHeadHandler::RequestHeadHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location), _reader(NULL)
{
	if (server->cgiEnabled() && server->isCGIextension(_resource_path))
	{
		// TODO: CGI 핸들러 완성시 주석 해제
		// _cgi_handler = new CGIHandler(args);
		return;
	}
	_reader = new async::FileReader(_server->_timeout_ms, _resource_path);
}

Server::RequestHeadHandler::~RequestHeadHandler()
{
	// TODO: CGI 핸들러 완성시 주석 해제
	// if (_cgi_handler)
	// 	delete _cgi_handler;
	if (_reader)
		delete _reader;
}

int Server::RequestHeadHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (_cgi_handler)
	{
		CGI::Request cgi_request;
		setCGIRequestValues(cgi_request);
		// _status = _cgi_handler->task();
	}

	if (_reader)
	{
		try
		{
			int rc = _reader->task();
			if (rc == async::status::OK)
			{
				const std::string &content = _reader->retrieve();
				_response.setStatus(200);
				_response.setContentLength(content.length());
				_response.setContentType(_resource_path);
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
		catch (const async::IOProcessor::FileIsDirectory &e)
		{
			registerErrorResponse(404, e); // Not Found
		}
		catch (const async::FileIOProcessor::FileOpeningError &e)
		{
			registerErrorResponse(404, e); // Not Found
		}
		catch (const std::exception &e)
		{
			registerErrorResponse(500, e); // Internal Server Error
		}
	}

	return (_status);
}
