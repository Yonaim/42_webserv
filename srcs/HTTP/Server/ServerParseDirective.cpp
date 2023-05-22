#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"
#include <iostream>
#include <sstream>

using namespace HTTP;

void Server::parseDirectiveListen(const ConfigContext &server_context)
{
	if (server_context.countDirectivesByName("listen") != 1)
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	const ConfigDirective &listen_directive
		= server_context.getNthDirectiveByName("listen", 0);
	if (listen_directive.is_context())
		listen_directive.throwException(PARSINGEXC_UNDEF_DIR);
	if (listen_directive.nParameters() != 1)
		listen_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	const std::string &port_str = listen_directive.parameter(0);
	if (!isUnsignedIntStr(port_str))
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	std::stringstream ss(port_str);
	ss >> _port;
	if (!(0 <= _port && _port <= 65536))
		listen_directive.throwException(PARSINGEXC_UNDEF_ARG);
	_logger << "parsed port " << _port << async::debug;
}

void Server::parseDirectiveRoot(const ConfigContext &server_context)
{
	if (server_context.countDirectivesByName("root") != 1)
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	const ConfigDirective &root_directive
		= server_context.getNthDirectiveByName("root", 0);
	if (root_directive.is_context())
		root_directive.throwException(PARSINGEXC_UNDEF_DIR);
	if (root_directive.nParameters() != 1)
		root_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	_root = root_directive.parameter(0);
	_logger << "parsed root " << _root << async::debug;
}

void Server::parseDirectiveErrorPage(const ConfigContext &server_context)
{
	const size_t n_error_pages
		= server_context.countDirectivesByName("error_page");
	if (n_error_pages == 0)
		return;
	for (size_t i = 0; i < n_error_pages; i++)
	{
		const ConfigDirective &error_page_directive
			= server_context.getNthDirectiveByName("error_page", i);
		if (error_page_directive.is_context())
			error_page_directive.throwException(PARSINGEXC_UNDEF_DIR);
		const size_t n_arguments = error_page_directive.nParameters();
		const std::string file_path
			= error_page_directive.parameter(n_arguments - 1);
		for (size_t i = 0; i < n_arguments - 1; i++)
		{
			const std::string &code_str = error_page_directive.parameter(i);
			if (!isUnsignedIntStr(code_str))
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			int code = toNum<int>(code_str);
			if (!isValidStatusCode(code))
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			// TODO: 타임아웃 정해야함
			_error_pages[code] = new async::FileReader(1000, _root + file_path);
			_logger << "parsed error page " << _root + file_path << " for code "
					<< code << async::debug;
		}
	}
}

void Server::parseDirectiveServerName(const ConfigContext &server_context)
{
	const size_t n_server_names
		= server_context.countDirectivesByName("server_name");
	if (n_server_names == 0)
		return;
	for (size_t i = 0; i < n_server_names; i++)
	{
		const ConfigDirective &server_name_directive
			= server_context.getNthDirectiveByName("server_name", i);
		if (server_name_directive.is_context())
			server_name_directive.throwException(PARSINGEXC_UNDEF_DIR);
		const size_t n_arguments = server_name_directive.nParameters();
		for (size_t i = 0; i < n_arguments; i++)
		{
			_server_name.insert(server_name_directive.parameter(i));
			_logger << "parsed server name "
					<< server_name_directive.parameter(i) << async::debug;
		}
	}
}

void Server::parseDirectiveLocation(const ConfigContext &server_context)
{
	const size_t n_locations = server_context.countDirectivesByName("location");
	if (n_locations == 0)
		return;
	for (size_t i = 0; i < n_locations; i++)
	{
		const ConfigContext &location_context
			= (const ConfigContext &)(server_context.getNthDirectiveByName(
				"location", i));
		if (!location_context.is_context())
			location_context.throwException(PARSINGEXC_UNDEF_DIR);

		Location new_location(location_context);
		if (_locations.find(new_location.getPath()) != _locations.end())
			location_context.throwException(PARSINGEXC_DUP_DIR);
		_locations[new_location.getPath()] = new_location;
		_logger << "parsed location " << new_location.getPath() << async::debug;
	}
}

bool Server::isValidStatusCode(const int &status_code)
{
	return (STATUS_CODE.find(status_code) != STATUS_CODE.end());
}
