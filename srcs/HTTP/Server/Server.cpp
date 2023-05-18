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

void HTTP::Server::registerRequest(const Request &request)
{
	(void)request;
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

HTTP::Response HTTP::Server::retrieveResponse(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
	return (HTTP::Response());
}

bool HTTP::Server::hasResponses(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
	return (false);
}
