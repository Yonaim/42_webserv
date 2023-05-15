#include "HTTPServer.hpp"
#include <map>

static const std::pair<std::string, bool> _http_server_directives_info[]
	= {std::pair<std::string, bool>("listen", false),
	   std::pair<std::string, bool>("server_name", false),
	   std::pair<std::string, bool>("error_page", false),
	   std::pair<std::string, bool>("location", true)};

static const std::pair<std::string, bool> _http_location_directives_info[]
	= {std::pair<std::string, bool>("root", false),
	   std::pair<std::string, bool>("limit_except", false),
	   std::pair<std::string, bool>("return", false),
	   std::pair<std::string, bool>("autoindex", false),
	   std::pair<std::string, bool>("index", false)};

const std::map<std::string, bool> HTTPServer::directives_info(
	_http_server_directives_info,
	_http_server_directives_info
		+ sizeof(_http_server_directives_info)
			  / sizeof(_http_server_directives_info[0]));

const std::map<std::string, bool> HTTPServer::HTTPLocation::directives_info(
	_http_location_directives_info,
	_http_location_directives_info
		+ sizeof(_http_location_directives_info)
			  / sizeof(_http_location_directives_info[0]));

static const std::pair<int, std::string> _http_methods[]
	= {std::pair<int, std::string>(GET, "GET"),
	   std::pair<int, std::string>(HEAD, "HEAD"),
	   std::pair<int, std::string>(POST, "POST"),
	   std::pair<int, std::string>(PUT, "PUT"),
	   std::pair<int, std::string>(DELETE, "DELETE"),
	   std::pair<int, std::string>(CONNECT, "CONNECT"),
	   std::pair<int, std::string>(OPTIONS, "OPTIONS"),
	   std::pair<int, std::string>(TRACE, "TRACE")};

const std::map<int, std::string> HTTPServer::http_methods(
	_http_methods,
	_http_methods + sizeof(_http_methods) / sizeof(_http_methods[0]));
