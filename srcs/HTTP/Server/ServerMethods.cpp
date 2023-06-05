#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/const_values.hpp"
#include "HTTP/error_pages.hpp"
#include "utils/string.hpp"
#include <cctype>
#include <climits>

using namespace HTTP;

void Server::ensureClientConnected(int client_fd) const
{
	if (_output_queue.find(client_fd) == _output_queue.end())
		throw(ClientNotFound(client_fd));
}

bool Server::isForMe(const Request &request) const
{
	if (request.countHeaderValue("Host") != 1)
	{
		throw(InvalidRequest("zero or more than one header "
							 "field found with name \"Host\""));
	}
	const std::string &host = request.getHeaderValue("Host", 0);
	LOG_VERBOSE("Request is for host \"" << host << "\"");
	for (std::set<std::string>::iterator it = _server_name.begin();
		 it != _server_name.end(); it++)
	{
		if (*it == host)
		{
			LOG_VERBOSE("Request host match with \"" << host << "\"");
			return (true);
		}
	}
	return (false);
}

bool Server::hasServerName(void) const
{
	return (_has_server_name);
}

bool Server::cgiAllowed(const int method) const
{
	bool method_allowed
		= _allowed_cgi_methods.find(method) != _allowed_cgi_methods.end();
	return (_cgi_enabled && method_allowed);
}

bool Server::hasResponses(int client_fd) const
{
	ensureClientConnected(client_fd);
	return (!_output_queue.find(client_fd)->second.empty());
}

int Server::hasResponses(void) const
{
	for (std::map<int, std::queue<Response> >::const_iterator it
		 = _output_queue.begin();
		 it != _output_queue.end(); it++)
	{
		if (!it->second.empty())
			return (it->first);
	}
	return (-1);
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

int Server::getPort(void) const
{
	return (_port);
}

unsigned int Server::getTimeout(void) const
{
	return (_timeout_ms);
}

std::string Server::getErrorPage(const int code)
{
	if (_error_page_paths.find(code) == _error_page_paths.end())
		return (generateErrorPage(code));

	_FileReaderPtr &reader = _error_page_readers[_error_page_paths[code]];
	int rc = reader->task();
	if (rc == async::status::OK_DONE)
		return (reader->retrieve());
	else if (rc == async::status::OK_AGAIN)
		return (generateErrorPage(code));
	else if (rc == async::status::ERROR_TIMEOUT)
		LOG_ERROR("Timeout while opening error page: " + reader->errorMsg());
	else
		LOG_ERROR("Unknown error while loading error page for code " << code);
	return ("");
}

const Server::Location &Server::getLocation(const std::string &path) const
{
	size_t cmp_diff;
	size_t cur_diff = ULLONG_MAX;
	std::map<std::string, Location>::const_iterator result;
	std::map<std::string, Location>::const_iterator it = _locations.begin();

	for (; it != _locations.end(); ++it)
	{
		const std::string location_param = it->first;

		if (path.find(location_param) == 0
			&& (path.length() == location_param.length()
				|| path[location_param.length()] == '/'
				|| path[location_param.length() - 1] == '/'))
		{
			cmp_diff = (path.length() - location_param.length());
			if (cmp_diff < cur_diff)
			{
				cur_diff = cmp_diff;
				result = it;
			}
		}
	}

	if (cur_diff == ULLONG_MAX)
		throw(LocationNotFound(path));
	LOG_VERBOSE("path " << path << " is for location " << result->first);
	return (result->second);
}
