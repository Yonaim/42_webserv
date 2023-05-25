#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestGetHandler::RequestGetHandler(Server *server,
											 const Request &request,
											 const Server::Location &location)
	: RequestHandler(server, request, location), _reader(NULL),
	  _cgi_handler(NULL)
{
	if (location.cgiEnabled() && location.isCGIextension(_resource_path))
	{
		// TODO: CGI 핸들러 완성시 주석 해제
		// _cgi_handler = new CGIHandler(args);
		return;
	}
	_reader = new async::FileReader(1000, _resource_path);
}

Server::RequestGetHandler::~RequestGetHandler()
{
	// TODO: CGI 핸들러 완성시 주석 해제
	// if (_cgi_handler)
	// 	delete _cgi_handler;
	if (_reader)
		delete _reader;
}

// uri = <스킴>://<사용자
// 이름>:<비밀번호>@<호스트>:<포트>/<경로>;<파라미터>?<질의>#<프래그먼트>

int Server::RequestGetHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	if (isInvalidDirectoryFormat())
	{
		_response = _server->generateErrorResponse(301); // Not Found;
		_response.setValue("Location", _request.getURIPath() + "/");
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		_server->_logger << "invalid directory format" << async::warning;
		return (_status);
	}
	if (_cgi_handler)
	{
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
				_response.setBody(content);
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
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			if (_location.hasAutoIndex() == true)
			{
				_response.makeDirectoryListing(_resource_path,
											   _request.getURIPath());
				_server->_logger << "directory listing" << async::verbose;
			}
			else
				registerErrorResponse(404, e); // Not Found
		}
		catch (const async::FileIOProcessor::FileOpeningError &e)
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			registerErrorResponse(404, e); // Not Found
		}
		catch (const std::exception &e)
		{
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
			registerErrorResponse(500, e); // Internal Server Error
		}
	}

	return (_status);
}
