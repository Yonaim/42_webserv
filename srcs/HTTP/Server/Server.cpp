#include "ConfigDirective.hpp"
#include "HTTPServer.hpp"
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

void HTTP::Server::task(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

bool isForMe(const HTTP::Request &request)
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
	if (_input_queue.find(client_fd) == _input_queue.end())
	{
		_input_queue[client_fd] = std::queue<HTTP::Request>();
		_output_queue[client_fd] = std::queue<HTTP::Response>();
	}
	_input_queue[client_fd].push(request);
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

bool HTTP::Server::hasResponses(void)
{
	for (std::map<int, std::queue<Response> >::iterator it
		 = _output_queue.begin();
		 it != _output_queue.end(); it++)
	{
		if (!it->second.empty())
			return (true);
	}
	return (false);
}

bool HTTP::Server::hasResponses(int client_fd)
{
	ensureClientConnected(client_fd);
	return (!_output_queue[client_fd].empty());
}

void HTTP::Server::disconnect(int client_fd)
{
	ensureClientConnected(client_fd);
	_input_queue.erase(client_fd);
	_output_queue.erase(client_fd);
}
