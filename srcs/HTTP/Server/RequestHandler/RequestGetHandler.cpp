#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestGetHandler::RequestGetHandler(Server const *server,
											 const Request &request)
	: RequestHandler(server, request), _reader(1000, _resource_path)
{
}

Server::RequestGetHandler::~RequestGetHandler()
{
}

// 호출 금지
Server::RequestGetHandler::RequestGetHandler(const RequestGetHandler &orig)
	: Server::RequestHandler(orig._server, orig._request), _reader(0, 0)
{
}

// 호출 금지
Server::RequestGetHandler &Server::RequestGetHandler::operator=(
	const RequestGetHandler &orig)
{
	(void)orig;
	return (*this);
}

// uri = <스킴>://<사용자
// 이름>:<비밀번호>@<호스트>:<포트>/<경로>;<파라미터>?<질의>#<프래그먼트>

int Server::RequestGetHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	int rc = _reader.task();
	if (rc == async::status::OK)
	{
		const std::string &content = _reader.retrieve();
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
		_response.setStatus(404);
		// _response.setBody(_error_pages.find(404)->second);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	return (_status);
}
