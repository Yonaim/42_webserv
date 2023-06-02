#include "WebServer.hpp"
#include "HTTP/error_pages.hpp"
#include "async/Logger.hpp"
#include "utils/string.hpp"

static const int _backlog_size_default = 8;
bool WebServer::_terminate = false;

WebServer::WebServer(const ConfigContext &root_context)
	: _backlog_size(_backlog_size_default),
	  _logger(async::Logger::getLogger("WebServer"))
{
	parseMaxBodySize(root_context);
	parseUploadStore(root_context);
	parseTimeout(root_context);
	parseBacklogSize(root_context);

	const char *dir_name = "server";
	size_t n_servers = root_context.countDirectivesByName(dir_name);
	if (n_servers < 1)
	{
		_logger << async::error << root_context.name()
				<< " should have 1 or more " << dir_name;
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));
	}
	for (size_t i = 0; i < n_servers; i++)
	{
		parseServer((const ConfigContext &)(root_context.getNthDirectiveByName(
			dir_name, i)));
	}
}

WebServer::~WebServer()
{
}
