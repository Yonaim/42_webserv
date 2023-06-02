#include "HTTP/Server.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/const_values.hpp"
#include "HTTP/error_pages.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

Server::Server(const ConfigContext &server_context, const size_t max_body_size,
			   const unsigned int timeout_ms)
	: _cgi_enabled(false), _temp_dir_path("."), _max_body_size(max_body_size),
	  _timeout_ms(timeout_ms), _logger(async::Logger::getLogger("Server"))
{
	parseDirectiveListen(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
	parseDirectiveCGI(server_context);
	parseDirectiveCGILimitExcept(server_context);
	parseDirectiveTmpDirPath(server_context);
}

Server::~Server()
{
}

Server::Server(const Server &orig)
	: _port(orig._port), _cgi_enabled(orig._cgi_enabled),
	  _server_name(orig._server_name), _error_pages(orig._error_pages),
	  _locations(orig._locations), _cgi_ext_to_path(orig._cgi_ext_to_path),
	  _temp_dir_path(orig._temp_dir_path),
	  _allowed_cgi_methods(orig._allowed_cgi_methods),
	  _max_body_size(orig._max_body_size), _timeout_ms(orig._timeout_ms),
	  _logger(async::Logger::getLogger("Server"))
{
}

Server &Server::operator=(const Server &orig)
{
	_port = orig._port;
	_cgi_enabled = orig._cgi_enabled;
	_server_name = orig._server_name;
	_error_pages = orig._error_pages;
	_locations = orig._locations;
	_cgi_ext_to_path = orig._cgi_ext_to_path;
	_temp_dir_path = orig._temp_dir_path;
	_allowed_cgi_methods = orig._allowed_cgi_methods;
	_max_body_size = orig._max_body_size;
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
	_logger << async::verbose << "path " << path << " is for location "
			<< result->first;
	return (result->second);
}

bool Server::isCGIextension(const std::string &path) const
{
	if (!_cgi_enabled)
		return (false);
	const std::string ext = getExtension(path);
	for (std::map<std::string, std::string>::const_iterator it
		 = _cgi_ext_to_path.begin();
		 it != _cgi_ext_to_path.end(); it++)
	{
		if (it->first == ext)
			return (true);
	}
	return (false);
}
