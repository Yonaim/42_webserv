#include "HTTPServer.hpp"
#include <map>

static const std::pair<std::string, int> __http_methods[]
	= {std::pair<std::string, int>("GET", GET),
	   std::pair<std::string, int>("HEAD", HEAD),
	   std::pair<std::string, int>("POST", POST),
	   std::pair<std::string, int>("PUT", PUT),
	   std::pair<std::string, int>("DELETE", DELETE),
	   std::pair<std::string, int>("CONNECT", CONNECT),
	   std::pair<std::string, int>("OPTIONS", OPTIONS),
	   std::pair<std::string, int>("TRACE", TRACE)};

const std::map<std::string, int> HTTPServer::_http_methods(
	__http_methods,
	__http_methods + sizeof(__http_methods) / sizeof(__http_methods[0]));
