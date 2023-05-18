#include "ConfigDirective.hpp"
#include "HTTPServer.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

static bool isUnsignedIntStr(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return (false);
	return (true);
}

void HTTP::Server::parseDirectiveListen(const ConfigContext &server_context)
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
}

void HTTP::Server::parseDirectiveErrorPage(const ConfigContext &server_context)
{
	const size_t n_error_pages
		= server_context.countDirectivesByName("error_page");
	if (n_error_pages == 0)
		return;
	std::stringstream ss;
	int code;
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
			ss.str(code_str);
			ss >> code;
			if (!isValidStatusCode(code))
				error_page_directive.throwException(PARSINGEXC_UNDEF_ARG);
			_error_pages[code] = file_path;
		}
	}
}

void HTTP::Server::parseDirectiveServerName(const ConfigContext &server_context)
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
			_server_name.insert(server_name_directive.parameter(i));
	}
}

void HTTP::Server::parseDirectiveLocation(const ConfigContext &server_context)
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
	}
}

bool HTTP::Server::isValidStatusCode(const int &status_code)
{
	return (_http_status_code.find(status_code) != _http_status_code.end());
}

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
