#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestGetHandler::RequestGetHandler(Server *server,
											 const Request &request,
											 const Server::Location &location,
											 const std::string &resource_path)
	: RequestHandler(server, request, location, resource_path),
	  _reader(_server->_timeout_ms, _resource_path)
{
}

Server::RequestGetHandler::~RequestGetHandler()
{
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
		_logger << async::warning << "invalid directory format, redirect to \""
				<< _request.getURIPath() + "\"";
		return (_status);
	}

	try
	{
		int rc = _reader.task();
		if (rc == async::status::OK_DONE)
		{
			const std::string &content = _reader.retrieve();
			_response.setStatus(200);
			_response.setBody(content);
			_response.setContentLength(content.length());
			_response.setContentType(_resource_path);
			_status = Server::RequestHandler::RESPONSE_STATUS_OK;
		}
		else if (rc == async::status::OK_AGAIN)
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
			_response.setStatus(200);
			_logger << async::verbose << "directory listing";
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
	return (_status);
}
