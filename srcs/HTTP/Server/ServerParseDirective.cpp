#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/const_values.hpp"
#include "utils/string.hpp"
#include <sstream>

using namespace HTTP;

void Server::parseDirectiveListen(const ConfigContext &server_context)
{
	const char *dir_name = "listen";
	if (server_context.countDirectivesByName("listen") != 1)
	{
		LOG_ERROR(server_context.name() << " should have 1 " << dir_name);
		throw(ConfigDirective::InvalidNumberOfArgument(server_context));
	}
	const ConfigDirective &listen_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (listen_directive.is_context())
	{
		LOG_ERROR(dir_name << " should not be context");
		throw(ConfigDirective::UndefinedDirective(listen_directive));
	}
	if (listen_directive.nParameters() != 1)
	{
		LOG_ERROR(dir_name << " should have 1 parameter(s)");
		throw(ConfigDirective::InvalidNumberOfArgument(listen_directive));
	}
	const std::string &port_str = listen_directive.parameter(0);
	if (!isUnsignedIntStr(port_str))
	{
		LOG_ERROR(dir_name << " should have integer form parameter");
		throw(ConfigDirective::UndefinedArgument(listen_directive));
	}
	std::stringstream ss(port_str);
	ss >> _port;
	if (!(0 <= _port && _port <= 65536))
	{
		LOG_ERROR(dir_name << " has invalid port number");
		throw(ConfigDirective::UndefinedArgument(listen_directive));
	}
	LOG_VERBOSE("parsed port " << _port);
}

void Server::parseDirectiveErrorPage(const ConfigContext &server_context)
{
	const char *dir_name = "error_page";
	const size_t n_error_pages = server_context.countDirectivesByName(dir_name);
	if (n_error_pages == 0)
		return;
	for (size_t i = 0; i < n_error_pages; i++)
	{
		const ConfigDirective &error_page_directive
			= server_context.getNthDirectiveByName(dir_name, i);
		if (error_page_directive.is_context())
		{
			LOG_ERROR(dir_name << " should not be context");
			throw(ConfigDirective::UndefinedDirective(error_page_directive));
		}
		const size_t n_arguments = error_page_directive.nParameters();
		const std::string file_path
			= error_page_directive.parameter(n_arguments - 1);
		for (size_t i = 0; i < n_arguments - 1; i++)
		{
			const std::string &code_str = error_page_directive.parameter(i);
			if (!isUnsignedIntStr(code_str))
			{
				LOG_ERROR(dir_name << " should have integer form parameter");
				throw(ConfigDirective::UndefinedArgument(error_page_directive));
			}
			const int code = toNum<int>(code_str);
			if (!isValidStatusCode(code))
			{
				LOG_ERROR(dir_name << " has invalid status code");
				throw(ConfigDirective::UndefinedArgument(error_page_directive));
			}

			_error_page_paths[code] = file_path;
			LOG_VERBOSE("parsed error page " << file_path << " for code "
											 << code);
		}
	}
}

void Server::parseDirectiveServerName(const ConfigContext &server_context)
{
	_has_server_name = false;
	const char *dir_name = "server_name";
	const size_t n_server_names
		= server_context.countDirectivesByName(dir_name);
	if (n_server_names == 0)
		return;
	_has_server_name = true;
	for (size_t i = 0; i < n_server_names; i++)
	{
		const ConfigDirective &server_name_directive
			= server_context.getNthDirectiveByName(dir_name, i);
		if (server_name_directive.is_context())
		{
			LOG_ERROR(dir_name << " should not be context");
			throw(ConfigDirective::UndefinedDirective(server_name_directive));
		}
		const size_t n_arguments = server_name_directive.nParameters();
		for (size_t i = 0; i < n_arguments; i++)
		{
			_server_name.insert(server_name_directive.parameter(i));
			LOG_VERBOSE("parsed server name "
						<< server_name_directive.parameter(i));
		}
	}
}

void Server::parseDirectiveLocation(const ConfigContext &server_context)
{
	const char *dir_name = "location";
	const size_t n_locations = server_context.countDirectivesByName("location");
	if (n_locations == 0)
		return;
	for (size_t i = 0; i < n_locations; i++)
	{
		const ConfigContext &location_context
			= (const ConfigContext &)(server_context.getNthDirectiveByName(
				"location", i));
		if (!location_context.is_context())
		{
			LOG_ERROR(dir_name << " should be context");
			throw(ConfigDirective::UndefinedDirective(location_context));
		}
		Location new_location(location_context, _max_body_size);
		if (_locations.find(new_location.getPath()) != _locations.end())
		{
			LOG_ERROR(dir_name << " has duplicate paths");
			throw(ConfigDirective::DuplicateArgument(location_context));
		}
		_locations[new_location.getPath()] = new_location;
		LOG_VERBOSE("parsed location " << new_location.getPath());
	}
}

void Server::parseDirectiveCGI(const ConfigContext &server_context)
{
	const char *dir_name = "cgi_pass";
	const size_t n_indexs = server_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	for (size_t i = 0; i < n_indexs; i++)
	{
		const ConfigDirective &cgi_directive
			= server_context.getNthDirectiveByName(dir_name, i);
		if (cgi_directive.is_context())
		{
			LOG_ERROR(dir_name << " should not be context");
			throw(ConfigDirective::UndefinedDirective(cgi_directive));
		}
		if (cgi_directive.nParameters() != 2)
		{
			LOG_ERROR(dir_name << " should have 2 parameter(s)");
			throw(ConfigDirective::InvalidNumberOfArgument(cgi_directive));
		}
		_cgi_ext_to_path[cgi_directive.parameter(0)]
			= cgi_directive.parameter(1);
		LOG_VERBOSE("CGI pass from URI *."
					<< cgi_directive.parameter(0) << " to exec "
					<< cgi_directive.parameter(1) << " enabled");
	}
	_cgi_enabled = true;
}

void Server::parseDirectiveCGILimitExcept(const ConfigContext &server_context)
{
	if (!_cgi_enabled)
		return;

	const char *dir_name = "cgi_limit_except";
	const size_t n_limit_excepts
		= server_context.countDirectivesByName(dir_name);
	if (n_limit_excepts != 1)
	{
		LOG_ERROR(server_context.name() << " should have 0 or 1 " << dir_name);
		throw(ConfigDirective::InvalidNumberOfArgument(server_context));
	}
	_allowed_cgi_methods.clear();

	for (size_t i = 0; i < n_limit_excepts; i++)
	{
		const ConfigDirective &limit_except_directive
			= server_context.getNthDirectiveByName(dir_name, i);
		if (limit_except_directive.is_context())
		{
			LOG_ERROR(dir_name << " should not be context");
			throw(ConfigDirective::UndefinedDirective(limit_except_directive));
		}

		const size_t n_methods = limit_except_directive.nParameters();
		if (n_methods == 0)
		{
			LOG_ERROR(dir_name << " should have more than 0 parameter(s)");
			throw(ConfigDirective::InvalidNumberOfArgument(
				limit_except_directive));
		}

		for (size_t j = 0; j < n_methods; j++)
		{
			const BidiMap<std::string, int>::const_iterator it
				= METHOD.find(limit_except_directive.parameter(j));
			if (it == METHOD.end())
			{
				LOG_ERROR(dir_name << " has invalid HTTP method");
				throw(
					ConfigDirective::UndefinedArgument(limit_except_directive));
			}

			const int method = it->second;
			LOG_VERBOSE("CGI Method " << limit_except_directive.parameter(j)
									  << " allowed");
			_allowed_cgi_methods.insert(method);
		}
	}
}

void Server::parseDirectiveTmpDirPath(const ConfigContext &server_context)
{
	const char *dir_name = "temp_dir_path";
	const size_t n_indexs = server_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	if (n_indexs > 1)
	{
		LOG_ERROR(server_context.name() << " should have 0 or 1 " << dir_name);
		throw(ConfigDirective::InvalidNumberOfArgument(server_context));
	}
	const ConfigDirective &tmp_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (tmp_directive.is_context())
	{
		LOG_ERROR(dir_name << " should not be context");
		throw(ConfigDirective::UndefinedDirective(tmp_directive));
	}
	if (tmp_directive.nParameters() != 1)
	{
		LOG_ERROR(dir_name << " should have 1 parameter(s)");
		throw(ConfigDirective::InvalidNumberOfArgument(tmp_directive));
	}
	_temp_dir_path = tmp_directive.parameter(0);
	LOG_VERBOSE("temporary file storage path set to: " << _temp_dir_path);
}

bool Server::isValidStatusCode(const int &status_code)
{
	return (STATUS_CODE.find(status_code) != STATUS_CODE.end());
}
