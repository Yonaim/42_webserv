#include "WebServer.hpp"
#include "HTTP/error_pages.hpp"
#include "async/Logger.hpp"
#include "utils/string.hpp"

static const int _backlog_size_default = 8;

WebServer::WebServer(void)
	: _backlog_size(_backlog_size_default),
	  _logger(async::Logger::getLogger("WebServer"))
{
}

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
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	for (size_t i = 0; i < n_servers; i++)
	{
		parseServer((const ConfigContext &)(root_context.getNthDirectiveByName(
			dir_name, i)));
	}
}

WebServer::WebServer(const WebServer &orig)
	: _tcp_procs(orig._tcp_procs), _servers(orig._servers),
	  _timeout_ms(orig._timeout_ms), _backlog_size(orig._backlog_size),
	  _logger(orig._logger)
{
}

WebServer::~WebServer()
{
	for (_TCPProcMap::iterator it = _tcp_procs.begin(); it != _tcp_procs.end();
		 it++)
		it->second.finalize(NULL);
}

WebServer &WebServer::operator=(const WebServer &orig)
{
	_tcp_procs = orig._tcp_procs;
	_servers = orig._servers;
	_timeout_ms = orig._timeout_ms;
	_backlog_size = orig._backlog_size;
	return (*this);
}
