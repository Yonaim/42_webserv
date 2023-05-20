#include "HTTP/RequestHandler.hpp"

using namespace HTTP;

Server::RequestHeadHandler::RequestHeadHandler(Server const *server,
											   const Request &request)
	: RequestHandler(server, request), _reader(1000, _resource_path)
{
}

Server::RequestHeadHandler::~RequestHeadHandler()
{
}

// 호출 금지
Server::RequestHeadHandler::RequestHeadHandler(const RequestHeadHandler &orig)
	: Server::RequestHandler(orig._server, orig._request), _reader(0, 0)
{
}

// 호출 금지
Server::RequestHeadHandler &Server::RequestHeadHandler::operator=(
	const RequestHeadHandler &orig)
{
	(void)orig;
	return (*this);
}

int Server::RequestHeadHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	int rc = _reader.task();
	if (rc == async::JobStatus::OK)
	{
		const std::string &content = _reader.retrieve();
		_response.setStatus(200);
		_response.setContentLength(content.length());
		_response.setContentType(_resource_path);
		_status = Server::RequestHandler::RESPONSE_STATUS_OK;
	}
	else if (rc == async::JobStatus::AGAIN)
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
