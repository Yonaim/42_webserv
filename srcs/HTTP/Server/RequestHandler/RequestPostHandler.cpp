#include "HTTP/RequestHandler.hpp"
#include "async/FileIOProcessor.hpp"

using namespace HTTP;

Server::RequestPostHandler::RequestPostHandler(Server const *server,
											   const Request &request)
	: RequestHandler(server, request),
	  _writer(1000, _resource_path, request.getBody())
{
}

Server::RequestPostHandler::~RequestPostHandler()
{
}

// 호출 금지
Server::RequestPostHandler::RequestPostHandler(const RequestPostHandler &orig)
	: Server::RequestHandler(orig._server, orig._request), _writer(0, 0, 0)
{
}

// 호출 금지
Server::RequestPostHandler &Server::RequestPostHandler::operator=(
	const RequestPostHandler &orig)
{
	(void)orig;
	return (*this);
}

int Server::RequestPostHandler::task(void)
{
	if (_status == Server::RequestHandler::RESPONSE_STATUS_OK)
		return (_status);

	int rc = _writer.task();
	if (rc == async::JobStatus::OK)
	{
		_response.setStatus(200);
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
