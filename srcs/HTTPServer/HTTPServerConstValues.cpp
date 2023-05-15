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

static const std::pair<std::string, int> _http_methods[]
	= {std::pair<std::string, int>("GET", GET),
	   std::pair<std::string, int>("HEAD", HEAD),
	   std::pair<std::string, int>("POST", POST),
	   std::pair<std::string, int>("PUT", PUT),
	   std::pair<std::string, int>("DELETE", DELETE),
	   std::pair<std::string, int>("CONNECT", CONNECT),
	   std::pair<std::string, int>("OPTIONS", OPTIONS),
	   std::pair<std::string, int>("TRACE", TRACE)};

const std::map<std::string, int> HTTPServer::http_methods(
	_http_methods,
	_http_methods + sizeof(_http_methods) / sizeof(_http_methods[0]));
