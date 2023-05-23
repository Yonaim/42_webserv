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

const Server::Location &Server::getLocation(
	const std::string &path) const
{
	std::map<std::string, Location>::const_iterator iter = _locations.begin();

	for (; iter != _locations.end(); ++iter)
	{
		const std::string key = iter->first;

		if (path.find(key) == 0)
			break;
	}

	if (iter == _locations.end())
		throw(LocationNotFound(path));
	return (iter->second);
}

std::string Server::getResourcePath(const Request &req) const
{
	const std::string &uri_path = req.getURIPath();
	const Location &location = getLocation(uri_path);

	if (uri_path.back() == '/' && location.hasIndex())
		return (location.getRoot() + uri_path + location.getNthIndex(0));

	return (location.getRoot() + uri_path);
}
