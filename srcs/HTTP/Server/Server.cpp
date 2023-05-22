#include "HTTP/Server.hpp"
#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "utils/string.hpp"
#include <cctype>

HTTP::Server::Server(const ConfigContext &server_context)
	: _root(""), _logger(async::Logger::getLogger("Server"))
{
	parseDirectiveListen(server_context);
	parseDirectiveRoot(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
}

HTTP::Server::~Server()
{
}

HTTP::Server::Server(const Server &orig)
	: _port(orig._port), _server_name(orig._server_name), _root(""),
	  _error_pages(orig._error_pages), _locations(orig._locations),
	  _logger(async::Logger::getLogger("Server"))
{
}

HTTP::Server &HTTP::Server::operator=(const Server &orig)
{
	_root = orig._root;
	_port = orig._port;
	_server_name = orig._server_name;
	_error_pages = orig._error_pages;
	_locations = orig._locations;
	return (*this);
}

int HTTP::Server::getPort(void) const
{
	return (_port);
}

void HTTP::Server::task(void)
{
	for (std::map<int, std::queue<RequestHandler *> >::iterator it
		 = _request_handlers.begin();
		 it != _request_handlers.end(); it++)
	{
		int client_fd = it->first;
		std::queue<RequestHandler *> &handlers = it->second;
		if (handlers.empty())
			continue;
		int rc = handlers.front()->task();
		if (rc == RequestHandler::RESPONSE_STATUS_OK)
		{
			_output_queue[client_fd].push(handlers.front()->retrieve());
			delete handlers.front();
			handlers.pop();
		}
		else if (rc == RequestHandler::RESPONSE_STATUS_AGAIN)
			continue;
		else
		{
			// TODO: 예외 처리
		}
	}
}

bool HTTP::Server::isForMe(const HTTP::Request &request)
{
	if (!request.hasHeaderValue("Host"))
		throw(std::runtime_error(
			"HTTP Request has no header field with name \"Host\""));
	// TODO: Host 헤더 필드가 여러개일 때 예외 처리
	const std::string &host = request.getHeaderValue("Host", 0);
	_logger << "Request is for host \"" << host << "\"" << async::debug;
	for (std::set<std::string>::iterator it = _server_name.begin();
		 it != _server_name.end(); it++)
	{
		if (*it == host)
		{
			_logger << "Request host match with \"" << host << "\""
					<< async::debug;
			return (true);
		}
	}
	return (false);
}

void HTTP::Server::ensureClientConnected(int client_fd)
{
	if (_output_queue.find(client_fd) == _output_queue.end())
	{
		throw(std::runtime_error("Client fd " + toStr(client_fd)
								 + " is not yet connected."));
	}
}

void HTTP::Server::registerRequest(int client_fd, const Request &request)
{
	RequestHandler *handler;
	switch (request.getMethod())
	{
	case METHOD_GET:
		handler = new RequestGetHandler(this, request);
		break;
	case METHOD_HEAD:
		handler = new RequestHeadHandler(this, request);
		break;
	case METHOD_POST:
		handler = new RequestPostHandler(this, request);
		break;
	case METHOD_DELETE:
		handler = new RequestDeleteHandler(this, request);
		break;
	default:
		throw(ServerException(501));
		// TODO:
		// 501 not implemented 에러 반환
		// 이전에 파싱한 부분 약간 수정해야할듯
		// 존재하지 않는 메소드는 METHOD_NONE으로 저장해놓고 여기서 에러
		// 핸들링하도록
	}

	if (_request_handlers.find(client_fd) == _request_handlers.end())
	{
		_request_handlers[client_fd] = std::queue<RequestHandler *>();
		_output_queue[client_fd] = std::queue<Response>();
	}
	_request_handlers[client_fd].push(handler);
	_logger << "Registered RequestHandler for "
			<< METHOD_STR[request.getMethod()] << async::debug;
}

HTTP::Response HTTP::Server::retrieveResponse(int client_fd)
{
	ensureClientConnected(client_fd);
	if (_output_queue[client_fd].empty())
		throw(std::runtime_error("No response made for client fd "
								 + toStr(client_fd)));
	HTTP::Response res = _output_queue[client_fd].front();
	_output_queue[client_fd].pop();
	return (res);
}

int HTTP::Server::hasResponses(void)
{
	for (std::map<int, std::queue<Response> >::iterator it
		 = _output_queue.begin();
		 it != _output_queue.end(); it++)
	{
		if (!it->second.empty())
			return (it->first);
	}
	return (-1);
}

bool HTTP::Server::hasResponses(int client_fd)
{
	ensureClientConnected(client_fd);
	return (!_output_queue[client_fd].empty());
}

void HTTP::Server::disconnect(int client_fd)
{
	ensureClientConnected(client_fd);
	_request_handlers.erase(client_fd);
	_output_queue.erase(client_fd);
}
