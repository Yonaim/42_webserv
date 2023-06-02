#include "HTTP/error_pages.hpp"
#include "WebServer.hpp"
#include "async/Logger.hpp"
#include "utils/string.hpp"

void WebServer::parseMaxBodySize(const ConfigContext &root_context)
{
	const char *dir_name = "client_max_body_size";

	if (root_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << async::error << root_context.name() << " should have 1 "
				<< dir_name;
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));
	}

	const ConfigDirective &body_size_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (body_size_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		throw(ConfigDirective::UndefinedDirective(root_context));
	}
	if (body_size_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		throw(ConfigDirective::InvalidNumberOfArgument(body_size_directive));
	}

	_max_body_size = toNum<size_t>(body_size_directive.parameter(0));
	_logger << async::info << "global max body size is " << _max_body_size;
}

void WebServer::parseUploadStore(const ConfigContext &root_context)
{
	const char *dir_name = "upload_store";

	if (root_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << async::error << root_context.name() << " should have 1 "
				<< dir_name;
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));
	}

	const ConfigDirective &upload_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (upload_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		throw(ConfigDirective::UndefinedDirective(root_context));
	}
	if (upload_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		throw(ConfigDirective::InvalidNumberOfArgument(upload_directive));
	}

	_upload_store = upload_directive.parameter(0);
	_logger << async::info << "upload store path set to " << _upload_store;
}

void WebServer::parseTimeout(const ConfigContext &root_context)
{
	const char *dir_name = "timeout";

	if (root_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << async::error << root_context.name() << " should have 1 "
				<< dir_name;
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));
	}

	const ConfigDirective &timeout_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (timeout_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		throw(ConfigDirective::UndefinedDirective(root_context));
	}
	if (timeout_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		throw(ConfigDirective::InvalidNumberOfArgument(timeout_directive));
	}

	_timeout_ms = toNum<unsigned int>(timeout_directive.parameter(0));
	_logger << async::info << "global timeout is " << _timeout_ms;
}

void WebServer::parseBacklogSize(const ConfigContext &root_context)
{
	const char *dir_name = "backlog_size";

	if (root_context.countDirectivesByName(dir_name) == 0)
	{
		_logger << async::info << "backlog size is " << _backlog_size
				<< " (default)";
		return;
	}
	if (root_context.countDirectivesByName(dir_name) > 1)
	{
		_logger << async::error << root_context.name() << " should have 0 or 1 "
				<< dir_name;
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));
	}

	const ConfigDirective &backlog_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (backlog_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		throw(ConfigDirective::UndefinedDirective(root_context));
	}
	if (backlog_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		throw(ConfigDirective::InvalidNumberOfArgument(backlog_directive));
	}

	_backlog_size = toNum<int>(backlog_directive.parameter(0));
	if (_backlog_size < 1 || _backlog_size > 128)
	{
		_logger << async::error << dir_name << " should be between 1 and 128";
		throw(ConfigDirective::InvalidNumberOfArgument(backlog_directive));
	}
	_logger << async::info << "backlog size is " << _backlog_size;
}

void WebServer::parseServer(const ConfigContext &server_context)
{
	HTTP::Server *server
		= new HTTP::Server(server_context, _max_body_size, _timeout_ms);
	int port = server->getPort();
	_logger << async::info << "Created Server at port " << port;
	if (_tcp_procs.find(port) == _tcp_procs.end())
	{
		_tcp_procs[port] = new async::TCPIOProcessor(port, _backlog_size);
		_tcp_procs[port]->initialize();
		_logger << async::verbose << "Created TCP IO Processor at port "
				<< port;
		_servers[port] = _Servers();
		_request_buffer[port] = _ReqBufFdMap();
	}
	_servers[port].push_back(server);
}
