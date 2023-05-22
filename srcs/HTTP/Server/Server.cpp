#include "HTTP/Server.hpp"
#include "../const_values.hpp"
#include "../error_pages.hpp"
#include "ConfigDirective.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

Server::Server(const ConfigContext &server_context)
	: _root(""), _logger(async::Logger::getLogger("Server"))
{
	parseDirectiveListen(server_context);
	parseDirectiveRoot(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
}

Server::~Server()
{
}

Server::Server(const Server &orig)
	: _port(orig._port), _server_name(orig._server_name), _root(""),
	  _error_pages(orig._error_pages), _locations(orig._locations),
	  _logger(async::Logger::getLogger("Server"))
{
}

Server &Server::operator=(const Server &orig)
{
	_root = orig._root;
	_port = orig._port;
	_server_name = orig._server_name;
	_error_pages = orig._error_pages;
	_locations = orig._locations;
	return (*this);
}
