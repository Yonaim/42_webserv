#include "WebServer.hpp"
#include "../HTTP/utils.hpp" // TODO: 공용 유틸 함수 헤더 만들기
#include "AsyncIOTaskHandler.hpp"
#include <iostream>

WebServer::WebServer(void)
{
}

void WebServer::parseMaxBodySize(const ConfigContext &root_context)
{
	const char *dir_name = "client_max_body_size";

	if (root_context.countDirectivesByName(dir_name) != 1)
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);

	const ConfigDirective &body_size_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (body_size_directive.is_context())
		root_context.throwException(PARSINGEXC_UNDEF_DIR);
	if (body_size_directive.nParameters() != 1)
		body_size_directive.throwException(PARSINGEXC_INVALID_N_ARG);

	_max_body_size = toNum<size_t>(body_size_directive.parameter(0));
}

void WebServer::parseUploadStore(const ConfigContext &root_context)
{
	const char *dir_name = "upload_store";

	if (root_context.countDirectivesByName(dir_name) != 1)
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);

	const ConfigDirective &upload_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (upload_directive.is_context())
		root_context.throwException(PARSINGEXC_UNDEF_DIR);
	if (upload_directive.nParameters() != 1)
		upload_directive.throwException(PARSINGEXC_INVALID_N_ARG);

	_upload_store = upload_directive.parameter(0);
}

void WebServer::parseServer(const ConfigContext &server_context)
{
	HTTP::Server server(server_context);
	int port = server.getPort();
	std::cout << "Created Server at port " << port << std::endl;
	if (_tcp_procs.find(port) == _tcp_procs.end())
	{
		_tcp_procs[port] = AsyncTCPIOProcessor(port);
		_tcp_procs[port].initialize();
		std::cout << "Created TCP IO Processor at port " << port << std::endl;
		_servers[port] = _Servers();
		_request_buffer[port] = _ReqBufFdMap();
	}
	_servers[port].push_back(server);
}

WebServer::WebServer(const ConfigContext &root_context)
{
	parseMaxBodySize(root_context);
	parseUploadStore(root_context);

	const char *dir_name = "server";
	size_t n_servers = root_context.countDirectivesByName(dir_name);
	if (n_servers < 1)
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);
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
	: _tcp_procs(orig._tcp_procs), _servers(orig._servers)
{
}

WebServer &WebServer::operator=(const WebServer &orig)
{
	_tcp_procs = orig._tcp_procs;
	_servers = orig._servers;
	return (*this);
}

void WebServer::parseRequestForEachFd(int port, AsyncTCPIOProcessor &tcp_proc)
{
	for (AsyncTCPIOProcessor::iterator it = tcp_proc.begin();
		 it != tcp_proc.end(); it++)
	{
		int client_fd = *it;
		if (tcp_proc.rdbuf(client_fd).empty())
			continue;

		if (_request_buffer[port].find(client_fd)
			== _request_buffer[port].end())
			_request_buffer[port][client_fd] = HTTP::Request();

		int rc
			= _request_buffer[port][client_fd].parse(tcp_proc.rdbuf(client_fd));

		switch (rc)
		{
		case HTTP::Request::RETURN_TYPE_OK:
			registerRequest(port, client_fd, _request_buffer[port][client_fd]);
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
	// TODO: 주인 없는 리퀘스트 처리
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
			HTTP::Response res = server_it->retrieveResponse(client_fd);
			_tcp_procs[port].wrbuf(client_fd) += res.toString();
		}
	}
}

void WebServer::task(void)
{
	AsyncIOTaskHandler::task();
	for (_TCPProcMap::iterator it = _tcp_procs.begin(); it != _tcp_procs.end();
		 it++)
		parseRequestForEachFd(it->first, it->second);
	for (_ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
		retrieveResponseForEachFd(it->first, it->second);
}
