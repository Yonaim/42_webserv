#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server *server,
											   const Request &request,
											   const Server::Location &location)
	: RequestHandler(server, request, location), _writer(NULL),
	  _cgi_handler(NULL)
{
	if (location.cgiEnabled() && location.isCGIextension(_resource_path))
	{
		// TODO: CGI 핸들러 완성시 주석 해제
		// _cgi_handler = new CGIHandler(args);
		return;
	}
	if (0)
	{
		_writer
			= new async::FileWriter(1000, _resource_path, request.getBody());
		return;
	}
	_response = _server->generateErrorResponse(500);
	_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	_server->_logger << "This location doesn't allow uploads or CGI"
					 << async::warning;
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
		// _status = _cgi_handler->task();
	}

	if (_writer)
	{
		try
		{
			int rc = _writer->task();
			if (rc == async::status::OK)
			{
				_response.setStatus(201); // Created
				_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			}
			else if (rc == async::status::AGAIN)
			{
				_status = Server::RequestHandler::RESPONSE_STATUS_AGAIN;
			}
			else
			{
				// TODO: 예외 처리
				_response.setStatus(404);
				// _response.setBody(_error_pages.find(404)->second);
				_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			}
		}
		catch (const async::FileIOProcessor::FileOpeningError &e)
		{
			// Service Unavailable
			_response = _server->generateErrorResponse(503);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			_server->_logger << e.what() << async::warning;
		}
		catch (const std::exception &e)
		{
			// Internal Server Error
			_response = _server->generateErrorResponse(500);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			_server->_logger << e.what() << async::warning;
		}
	}

	return (_status);
}
