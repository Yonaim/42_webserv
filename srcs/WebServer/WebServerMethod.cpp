#include "HTTP/ParsingFail.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/error_pages.hpp"
#include "WebServer.hpp"
#include "async/Logger.hpp"
#include "utils/string.hpp"

void WebServer::setTerminationFlag(void)
{
	WebServer::_terminate = true;
}

void WebServer::parseRequestForEachFd(int port, async::TCPIOProcessor &tcp_proc)
{
	for (async::TCPIOProcessor::iterator it = tcp_proc.begin();
		 it != tcp_proc.end(); it++)
	{
		int client_fd = *it;
		if (tcp_proc.rdbuf(client_fd).empty())
			continue;

		std::map<int, HTTP::Request> &requests
			= _request_buffer.find(port)->second;
		if (requests.find(client_fd) == requests.end())
			resetRequestBuffer(port, client_fd);

		int rc;
		try
		{
			rc = getRequestBuffer(port, client_fd)
					 .parse(tcp_proc.rdbuf(client_fd));
		}
		catch (const HTTP::ParsingFail &e)
		{
			// TODO: 오류 상황에 따라 에러 코드 세분화
			// TODO: 에러 코드에 따라 연결 끊을 수도 있게 처리
			_logger << async::warning << "Parsing failure: " << e.what();
			resetRequestBuffer(port, client_fd);
			HTTP::Response res = generateErrorResponse(400); // Bad Request
			_tcp_procs[port]->wrbuf(client_fd) += res.toString();
			_logger << async::debug << "Added to wrbuf: \"" << res.toString()
					<< "\"";
			continue;
		}

		switch (rc)
		{
		case HTTP::Request::RETURN_TYPE_OK:
			_logger << async::info << "Inbound request "
					<< getRequestBuffer(port, client_fd);
			registerRequest(port, client_fd, getRequestBuffer(port, client_fd));
			resetRequestBuffer(port, client_fd);
			break;

		case HTTP::Request::RETURN_TYPE_AGAIN:
			// let it run again at next call
			break;

		default:
			_logger << async::warning << "Unknown parsing error";
			resetRequestBuffer(port, client_fd);
			HTTP::Response res = generateErrorResponse(500);
			_tcp_procs[port]->wrbuf(client_fd) += res.toString();
			_logger << async::debug << "Added to wrbuf: \"" << res.toString()
					<< "\"";
			break;
		}
	}
}

WebServer::_Servers::iterator WebServer::findNoneNameServer(int port)
{
	for (_Servers::iterator it = _servers[port].begin();
		 it != _servers[port].end(); it++)
	{
		if ((*it)->hasServerName() == false)
			return (it);
	}
	return (_servers[port].end());
}

HTTP::Request &WebServer::getRequestBuffer(int port, int client_fd)
{
	return (_request_buffer.find(port)->second.find(client_fd)->second);
}

void WebServer::resetRequestBuffer(int port, int client_fd)
{
	_request_buffer.find(port)->second.erase(client_fd);
	_request_buffer.find(port)->second.insert(
		std::pair<int, HTTP::Request>(client_fd, HTTP::Request()));
}

void WebServer::registerRequest(int port, int client_fd, HTTP::Request &request)
{
	try
	{
		for (_Servers::iterator it = _servers[port].begin();
			 it != _servers[port].end(); it++)
		{
			if ((*it)->isForMe(request))
			{
				(*it)->registerRequest(client_fd, request);
				return;
			}
		}
	}
	catch (const HTTP::Server::InvalidRequest &e)
	{
		_logger << async::warning << e.what();
	}
	_logger << async::warning << "No matching server for "
			<< request.getHeaderValue("Host", 0);
	// 일치하는 Host가 없을 시, 해당 포트의 server_name이 없는 서버를 찾아보고
	// 그러한 서버가 없다면 해당 포트의 첫 서버에 등록
	if (findNoneNameServer(port) != _servers[port].end())
		(*findNoneNameServer(port))->registerRequest(client_fd, request);
	else
		_servers[port].front()->registerRequest(client_fd, request);
}

void WebServer::retrieveResponseForEachFd(int port, _Servers &servers)
{
	for (_Servers::iterator server_it = servers.begin();
		 server_it != servers.end(); server_it++)
	{
		HTTP::Server *server = *server_it;
		server->task();
		while (true)
		{
			int client_fd = server->hasResponses();
			if (client_fd < 0)
				break;
			_logger << async::verbose << "Response for client " << client_fd
					<< " has been found";
			HTTP::Response res = server->retrieveResponse(client_fd);
			_tcp_procs[port]->wrbuf(client_fd) += res.toString();
			_logger << async::debug << "Added to wrbuf: \"" << res.toString()
					<< "\"";
			_logger << async::info << "Outbound response " << res;
		}
	}
}

HTTP::Response WebServer::generateErrorResponse(const int code)
{
	HTTP::Response response;

	std::string body = HTTP::generateErrorPage(code);
	response.setStatus(code);
	response.setBody(body);
	response.setContentLength(body.length());
	response.setContentType("text/html");
	return (response);
}

void WebServer::disconnect(int port, int client_fd)
{
	_request_buffer[port].erase(client_fd);
	for (_Servers::iterator it = _servers[port].begin();
		 it != _servers[port].end(); it++)
	{
		try
		{
			(*it)->disconnect(client_fd);
		}
		catch (const HTTP::Server::ClientNotFound &e)
		{
			_logger << async::warning << e.what();
		}
	}
	_logger << async::info << "Disconnected client fd " << client_fd
			<< " from port " << port;
}

void WebServer::terminate(void)
{
	while (!_tcp_procs.empty())
	{
		int port = _tcp_procs.begin()->first;
		async::TCPIOProcessor *tcp = _tcp_procs.begin()->second;

		tcp->finalize(NULL);
		while (!tcp->disconnected_clients.empty())
		{
			int disconnected_fd = tcp->disconnected_clients.front();
			tcp->disconnected_clients.pop();
			disconnect(port, disconnected_fd);
		}
		delete tcp;
		_tcp_procs.erase(_tcp_procs.begin());
	}
}

int WebServer::task(void)
{
	if (_terminate)
	{
		terminate();
		return (async::status::OK_DONE);
	}

	async::IOProcessor::doAllTasks();
	for (_TCPProcMap::iterator it = _tcp_procs.begin(); it != _tcp_procs.end();
		 it++)
	{
		int port = it->first;
		async::TCPIOProcessor &tcp = *(it->second);

		while (!tcp.disconnected_clients.empty())
		{
			int disconnected_fd = tcp.disconnected_clients.front();
			tcp.disconnected_clients.pop();
			disconnect(port, disconnected_fd);
		}
		parseRequestForEachFd(port, tcp);
	}
	for (_ServerMap::iterator it = _servers.begin(); it != _servers.end(); it++)
		retrieveResponseForEachFd(it->first, it->second);
	return (async::status::OK_AGAIN);
}
