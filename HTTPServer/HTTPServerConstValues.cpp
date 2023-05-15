#include "HTTPServer.hpp"
#include <map>

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
