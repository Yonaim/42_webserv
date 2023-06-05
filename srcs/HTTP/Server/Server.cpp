#include "HTTP/Server.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/const_values.hpp"
#include "HTTP/error_pages.hpp"
#include "utils/string.hpp"
#include <cctype>

using namespace HTTP;

const int Server::_http_min_version = 1001;
const int Server::_http_max_version = 1001;

Server::Server(const ConfigContext &server_context, const size_t max_body_size,
			   const unsigned int timeout_ms)
	: _cgi_enabled(false), _temp_dir_path("."), _max_body_size(max_body_size),
	  _timeout_ms(timeout_ms), _logger(async::Logger::getLogger("Server"))
{
	parseDirectiveListen(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
	parseDirectiveCGI(server_context);
	parseDirectiveCGILimitExcept(server_context);
	parseDirectiveTmpDirPath(server_context);
}

Server::~Server()
{
}
