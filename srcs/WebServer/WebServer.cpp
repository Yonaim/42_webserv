#include "WebServer.hpp"
#include "async/IOTaskHandler.hpp"
#include "async/Logger.hpp"
#include "utils/string.hpp"

WebServer::WebServer(void) : _logger(async::Logger::getLogger("WebServer"))
{
}

void WebServer::parseMaxBodySize(const ConfigContext &root_context)
{
	const char *dir_name = "client_max_body_size";

	if (root_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << root_context.name() << " should have 1 " << dir_name
				<< async::error;
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}

	const ConfigDirective &body_size_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (body_size_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		root_context.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (body_size_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		body_size_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}

	_max_body_size = toNum<size_t>(body_size_directive.parameter(0));
}

void WebServer::parseUploadStore(const ConfigContext &root_context)
{
	const char *dir_name = "upload_store";

	if (root_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << root_context.name() << " should have 1 " << dir_name
				<< async::error;
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}

	const ConfigDirective &upload_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (upload_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		root_context.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (upload_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		upload_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}

	_upload_store = upload_directive.parameter(0);
}

void WebServer::parseServer(const ConfigContext &server_context)
{
	HTTP::Server server(server_context);
	int port = server.getPort();
	_logger << "Created Server at port " << port << async::verbose;
	if (_tcp_procs.find(port) == _tcp_procs.end())
	{
		_tcp_procs[port] = async::TCPIOProcessor(port);
		_tcp_procs[port].initialize();
		_logger << "Created TCP IO Processor at port " << port
				<< async::verbose;
		_servers[port] = _Servers();
		_request_buffer[port] = _ReqBufFdMap();
	}
	_servers[port].push_back(server);
}

WebServer::WebServer(const ConfigContext &root_context)
	: _logger(async::Logger::getLogger("WebServer"))
{
	parseMaxBodySize(root_context);
	parseUploadStore(root_context);

	const char *dir_name = "server";
	size_t n_servers = root_context.countDirectivesByName(dir_name);
	if (n_servers < 1)
	{
		_logger << root_context.name() << " should have 1 or more " << dir_name
				<< async::error;
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	for (size_t i = 0; i < n_servers; i++)
	{
		parseServer((const ConfigContext &)(root_context.getNthDirectiveByName(
			dir_name, i)));
	}
}

WebServer::~WebServer()
{
	for (_TCPProcMap::iterator it = _tcp_procs.begin(); it != _tcp_procs.end();
		 it++)
		it->second.finalize(NULL);
}

WebServer::WebServer(const WebServer &orig)
	: _tcp_procs(orig._tcp_procs), _servers(orig._servers),
	  _logger(orig._logger)
{
}

WebServer &WebServer::operator=(const WebServer &orig)
{
	_tcp_procs = orig._tcp_procs;
	_servers = orig._servers;
	return (*this);
}

void WebServer::parseRequestForEachFd(int port, async::TCPIOProcessor &tcp_proc)
{
	for (async::TCPIOProcessor::iterator it = tcp_proc.begin();
		 it != tcp_proc.end(); it++)
	{
		int client_fd = *it;
		if (tcp_proc.rdbuf(client_fd).empty())
			continue;

		if (_request_buffer[port].find(client_fd)
			== _request_buffer[port].end())
			_request_buffer[port][client_fd] = HTTP::Request();

		int rc
			= _request_buffer[port][client_fd].parse(tcp_proc.rdbuf(client_fd), _max_body_size);

		switch (rc)
		{
		case HTTP::Request::RETURN_TYPE_OK:
			registerRequest(port, client_fd, _request_buffer[port][client_fd]);
			_request_buffer[port][client_fd] = HTTP::Request();
			break;

		case HTTP::Request::RETURN_TYPE_AGAIN:
			// let it run again at next call
			break;

		default:
			// handle error
			break;
		}
	}
}

void WebServer::registerRequest(int port, int client_fd, HTTP::Request &request)
{
	for (_Servers::iterator it = _servers[port].begin();
		 it != _servers[port].end(); it++)
	{
		if (it->isForMe(request))
		{
			it->registerRequest(client_fd, request);
			return;
		}
	}
	// 일치하는 Host가 없을 시 해당 포트의 첫 서버(기본값)에 등록
	_servers[port].front().registerRequest(client_fd, request);
}

void WebServer::retrieveResponseForEachFd(int port, _Servers &servers)
{
	for (_Servers::iterator server_it = servers.begin();
		 server_it != servers.end(); server_it++)
	{
		server_it->task();
		while (true)
		{
			int client_fd = server_it->hasResponses();
			if (client_fd < 0)
				break;
			_logger << "Response for client " << client_fd << " has been found"
					<< async::verbose;
			HTTP::Response res = server_it->retrieveResponse(client_fd);
			_tcp_procs[port].wrbuf(client_fd) += res.toString();
			_logger << "Added to wrbuf: \"" << res.toString() << "\""
					<< async::debug;
		}
	}
}

void WebServer::task(void)
{
	async::IOTaskHandler::task();
	for (_TCPProcMap::iterator it = _tcp_procs.begin(); it != _tcp_procs.end();
		 it++)
		parseRequestForEachFd(it->first, it->second);
	for (_ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
		retrieveResponseForEachFd(it->first, it->second);
}
