#include "../const_values.hpp"
#include "HTTP/Server.hpp"
#include "ConfigDirective.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

HTTP::Server::Server(const ConfigContext &server_context)
{
	parseDirectiveListen(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
}

HTTP::Server::~Server()
{
}

HTTP::Server::Server(const Server &orig)
	: _port(orig._port), _server_name(orig._server_name),
	  _error_pages(orig._error_pages), _locations(orig._locations)
{
}

HTTP::Server &HTTP::Server::operator=(const Server &orig)
{
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

		int rc = handlers.front()->task();
		if (rc == RequestHandler::RESPONSE_STATUS_OK)
		{
			_output_queue[client_fd].push(handlers.front()->retrieve());
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
	(void)request;
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
	return (false);
}

void HTTP::Server::ensureClientConnected(int client_fd)
{
	if (_output_queue.find(client_fd) == _output_queue.end())
	{
		std::stringstream what;
		what << "Client fd " << client_fd << " is not yet connected.";
		throw(std::runtime_error(what.str()));
	}
}

void HTTP::Server::registerRequest(int client_fd, const Request &request)
{
	if (_request_handlers.find(client_fd) == _request_handlers.end())
	{
		_request_handlers[client_fd] = std::queue<RequestHandler *>();
		_output_queue[client_fd] = std::queue<Response>();
	}

	RequestHandler *handler;
	switch (request.getMethod())
	{
	case METHOD_GET:
		handler = new RequestGetHandler(request);
		break;
	case METHOD_HEAD:
		handler = new RequestHeadHandler(request);
		break;
	case METHOD_POST:
		handler = new RequestPostHandler(request);
		break;
	case METHOD_DELETE:
		handler = new RequestDeleteHandler(request);
		break;
	default:
		throw(ServerException(501));
		// TODO:
		// 501 not implemented 에러 반환
		// 이전에 파싱한 부분 약간 수정해야할듯
		// 존재하지 않는 메소드는 METHOD_NONE으로 저장해놓고 여기서 에러
		// 핸들링하도록
	}
	_request_handlers[client_fd].push(handler);
}

HTTP::Response HTTP::Server::retrieveResponse(int client_fd)
{
	ensureClientConnected(client_fd);
	if (_output_queue[client_fd].empty())
	{
		std::stringstream what;
		what << "No response made for client fd " << client_fd;
		throw(std::runtime_error(what.str()));
	}
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
