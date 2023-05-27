#include "HTTP/Server.hpp"
#include "../const_values.hpp"
#include "../error_pages.hpp"
#include "ConfigDirective.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

Server::Server(const ConfigContext &server_context)
	: _cgi_enabled(false), _logger(async::Logger::getLogger("Server"))
{
	parseDirectiveListen(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
	parseDirectiveCGI(server_context);
}

Server::~Server()
{
}

Server::Server(const Server &orig)
	: _port(orig._port), _cgi_enabled(orig._cgi_enabled),
	  _server_name(orig._server_name), _error_pages(orig._error_pages),
	  _locations(orig._locations), _logger(async::Logger::getLogger("Server"))
{
}

Server &Server::operator=(const Server &orig)
{
	_port = orig._port;
	_cgi_enabled = orig._cgi_enabled;
	_server_name = orig._server_name;
	_error_pages = orig._error_pages;
	_locations = orig._locations;
	return (*this);
}

const Server::Location &Server::getLocation(const std::string &path) const
{
	size_t cmp_diff;
	size_t cur_diff = ULLONG_MAX;
	std::map<std::string, Location>::const_iterator result;
	std::map<std::string, Location>::const_iterator iter = _locations.begin();

	for (; iter != _locations.end(); ++iter)
	{
		const std::string location_param = iter->first;

		if (path.find(location_param) == 0
			&& (path.length() == location_param.length()
				|| path[location_param.length()] == '/'
				|| path[location_param.length() - 1] == '/'))
		{
			cmp_diff = (path.length() - location_param.length());
			if (cmp_diff < cur_diff)
			{
				cur_diff = cmp_diff;
				result = iter;
			}
		}
	}

	if (cur_diff == ULLONG_MAX)
		throw(LocationNotFound(path));
	return (result->second);
}

std::string Server::getResourcePath(const Request &req) const
{
	std::string uri_path = req.getURIPath();
	const Location &location = getLocation(uri_path);
	const std::string &alias = location.getAlias();

	uri_path.replace(0, location.getPath().length(), alias);
	if (req.getURIPath().back() == '/' && location.hasIndex())
		uri_path += location.getNthIndex(0);

	return (uri_path);
}

bool Server::isCGIextension(const std::string &path) const
{
	std::string ext = getExtension(path);
	if (ext == "")
		return (false);
	return (_cgi_extensions.find(ext) != _cgi_extensions.end());
}
