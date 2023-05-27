#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"
#include <iostream>
#include <sstream>

using namespace HTTP;

void Server::parseDirectiveListen(const ConfigContext &server_context)
{
	const char *dir_name = "listen";
	if (server_context.countDirectivesByName("listen") != 1)
	{
		_logger << server_context.name() << " should have 1 " << dir_name
				<< async::error;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	const ConfigDirective &listen_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (listen_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		listen_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (listen_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		listen_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	const std::string &port_str = listen_directive.parameter(0);
	if (!isUnsignedIntStr(port_str))
	{
		_logger << dir_name << " should have integer form parameter"
				<< async::error;
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
	std::stringstream ss(port_str);
	ss >> _port;
	if (!(0 <= _port && _port <= 65536))
	{
		_logger << dir_name << " has invalid port number" << async::error;
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
	_logger << "parsed port " << _port << async::verbose;
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
			_logger << dir_name << " should not be context" << async::error;
			error_page_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}
		const size_t n_arguments = error_page_directive.nParameters();
		const std::string file_path
			= error_page_directive.parameter(n_arguments - 1);
		for (size_t i = 0; i < n_arguments - 1; i++)
		{
			const std::string &code_str = error_page_directive.parameter(i);
			if (!isUnsignedIntStr(code_str))
			{
				_logger << dir_name << " should have integer form parameter"
						<< async::error;
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}
			int code = toNum<int>(code_str);
			if (!isValidStatusCode(code))
			{
				_logger << dir_name << " has invalid status code"
						<< async::error;
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}
			_error_pages[code] = new async::FileReader(_timeout_ms, file_path);
			_logger << "parsed error page " << file_path << " for code " << code
					<< async::verbose;
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
			_logger << dir_name << " should not be context" << async::error;
			server_name_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}
		const size_t n_arguments = server_name_directive.nParameters();
		for (size_t i = 0; i < n_arguments; i++)
		{
			_server_name.insert(server_name_directive.parameter(i));
			_logger << "parsed server name "
					<< server_name_directive.parameter(i) << async::verbose;
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
			_logger << dir_name << " should be context" << async::error;
			location_context.throwException(PARSINGEXC_UNDEF_DIR);
		}
		Location new_location(location_context, _max_body_size);
		if (_locations.find(new_location.getPath()) != _locations.end())
		{
			_logger << dir_name << " has duplicate paths" << async::error;
			location_context.throwException(PARSINGEXC_DUP_DIR);
		}
		_locations[new_location.getPath()] = new_location;
		_logger << "parsed location " << new_location.getPath()
				<< async::verbose;
	}
}

void Server::parseDirectiveCGI(const ConfigContext &server_context)
{
	const char *dir_name = "cgi_extension";
	const size_t n_indexs = server_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	if (n_indexs > 1)
	{
		_logger << server_context.name() << " should have 0 or 1 " << dir_name
				<< async::error;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_cgi_enabled = true;
	const ConfigDirective &cgi_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (cgi_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		cgi_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	// 맨대토리 버젼, 1개의 아규먼트만 허용
	if (cgi_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		cgi_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_cgi_extensions.insert(cgi_directive.parameter(0));
	_logger << "CGI call in " << cgi_directive.parameter(0) << " enabled"
			<< async::verbose;
	/*
	보너스 대비한 버젼
	if (cgi_directive.nParameters() < 1)
	{
		_logger << dir_name << " should have more than 0 parameter(s)"
				<< async::error;
		cgi_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	for (size_t i = 0; i < cgi_directive.nParameters(); i++)
	{
		_cgi_extensions.insert(cgi_directive.parameter(i));
		_logger << "CGI call to in " << cgi_directive.parameter(i) << " enabled"
				<< async::verbose;
	}
	*/
}

bool Server::isValidStatusCode(const int &status_code)
{
	return (STATUS_CODE.find(status_code) != STATUS_CODE.end());
}
