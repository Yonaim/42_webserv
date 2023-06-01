#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/const_values.hpp"
#include "utils/string.hpp"
#include <iostream>
#include <sstream>

using namespace HTTP;

void Server::parseDirectiveListen(const ConfigContext &server_context)
{
	const char *dir_name = "listen";
	if (server_context.countDirectivesByName("listen") != 1)
	{
		_logger << async::error << server_context.name() << " should have 1 "
				<< dir_name;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	const ConfigDirective &listen_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (listen_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		listen_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (listen_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		listen_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	const std::string &port_str = listen_directive.parameter(0);
	if (!isUnsignedIntStr(port_str))
	{
		_logger << async::error << dir_name
				<< " should have integer form parameter";
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
	std::stringstream ss(port_str);
	ss >> _port;
	if (!(0 <= _port && _port <= 65536))
	{
		_logger << async::error << dir_name << " has invalid port number";
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
	_logger << async::verbose << "parsed port " << _port;
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
			_logger << async::error << dir_name << " should not be context";
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
				_logger << async::error << dir_name
						<< " should have integer form parameter";
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}
			int code = toNum<int>(code_str);
			if (!isValidStatusCode(code))
			{
				_logger << async::error << dir_name
						<< " has invalid status code";
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}
			_error_pages[code] = new async::FileReader(_timeout_ms, file_path);
			_logger << async::verbose << "parsed error page " << file_path
					<< " for code " << code;
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
			_logger << async::error << dir_name << " should not be context";
			server_name_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}
		const size_t n_arguments = server_name_directive.nParameters();
		for (size_t i = 0; i < n_arguments; i++)
		{
			_server_name.insert(server_name_directive.parameter(i));
			_logger << async::verbose << "parsed server name "
					<< server_name_directive.parameter(i);
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
			_logger << async::error << dir_name << " should be context";
			location_context.throwException(PARSINGEXC_UNDEF_DIR);
		}
		Location new_location(location_context, _max_body_size);
		if (_locations.find(new_location.getPath()) != _locations.end())
		{
			_logger << async::error << dir_name << " has duplicate paths";
			location_context.throwException(PARSINGEXC_DUP_DIR);
		}
		_locations[new_location.getPath()] = new_location;
		_logger << async::verbose << "parsed location "
				<< new_location.getPath();
	}
}

void Server::parseDirectiveCGI(const ConfigContext &server_context)
{
	const char *dir_name = "cgi_pass";
	const size_t n_indexs = server_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	if (n_indexs > 1) // 맨대토리 버젼, 1개의 확장자만 허용
	{
		_logger << async::error << server_context.name()
				<< " should have 0 or 1 " << dir_name;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_cgi_enabled = true;
	const ConfigDirective &cgi_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (cgi_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		cgi_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (cgi_directive.nParameters() != 2)
	{
		_logger << async::error << dir_name << " should have 2 parameter(s)";
		cgi_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_cgi_extension = cgi_directive.parameter(0);
	_cgi_exec_path = cgi_directive.parameter(1);
	_logger << async::verbose << "CGI pass from URI *." << _cgi_extension
			<< " to exec " << _cgi_exec_path << " enabled";
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
		_logger << async::error << server_context.name()
				<< " should have 0 or 1 " << dir_name;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_allowed_cgi_methods.clear();

	for (size_t i = 0; i < n_limit_excepts; i++)
	{
		const ConfigDirective &limit_except_directive
			= server_context.getNthDirectiveByName(dir_name, i);
		if (limit_except_directive.is_context())
		{
			_logger << async::error << dir_name << " should not be context";
			limit_except_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}

		const size_t n_methods = limit_except_directive.nParameters();
		if (n_methods == 0)
		{
			_logger << async::error << dir_name
					<< " should have more than 0 parameter(s)";
			limit_except_directive.throwException(PARSINGEXC_INVALID_N_ARG);
		}

		for (size_t j = 0; j < n_methods; j++)
		{
			const BidiMap<std::string, int>::const_iterator it
				= METHOD.find(limit_except_directive.parameter(j));
			if (it == METHOD.end())
			{
				_logger << async::error << dir_name
						<< " has invalid HTTP method";
				limit_except_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}

			const int method = it->second;
			_logger << async::verbose << "CGI Method "
					<< limit_except_directive.parameter(j) << " allowed";
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
		_logger << async::error << server_context.name()
				<< " should have 0 or 1 " << dir_name;
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	const ConfigDirective &tmp_directive
		= server_context.getNthDirectiveByName(dir_name, 0);
	if (tmp_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		tmp_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (tmp_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		tmp_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_temp_dir_path = tmp_directive.parameter(0);
	_logger << async::verbose
			<< "temporary file storage path set to: " << _temp_dir_path;
}

bool Server::isValidStatusCode(const int &status_code)
{
	return (STATUS_CODE.find(status_code) != STATUS_CODE.end());
}
