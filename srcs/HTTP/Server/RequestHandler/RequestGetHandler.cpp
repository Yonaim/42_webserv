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

	if (_cgi_handler)
	{
		// _status = _cgi_handler->task();
	}

	if (_reader)
	{
		/* TODO: 현재 얻어진 resourcePath가 유효하지 않고, _has_index =
		 * true;인경우 모든 index를 유효한 경로 후보로 두고 resourcePath를 새로
		 * 구하는 로직 추가*/
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
				// TODO: 예외 처리
			}
		}
		catch (const async::FileIOProcessor::FileOpeningError &e)
		{
			_response = _server->generateErrorResponse(404); // Not Found;
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
