#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "ConfigDirective.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/Logger.hpp"
#include "async/TCPIOProcessor.hpp"
#include "utils/shared_ptr.hpp"
#include <map>
#include <string>
#include <vector>

class WebServer
{
  private:
	typedef ft::shared_ptr<async::TCPIOProcessor> _TCPPtr;
	typedef ft::shared_ptr<HTTP::Server> _ServerPtr;

	typedef std::map<int, _TCPPtr> _TCPProcMap;
	typedef std::vector<_ServerPtr> _Servers;
	typedef std::map<int, _Servers> _ServerMap;
	typedef std::map<int, HTTP::Request> _ReqBufFdMap;
	typedef std::map<int, _ReqBufFdMap> _ReqBufPortMap;

	static bool _terminate;

	size_t _max_body_size;
	std::string _upload_store;
	_TCPProcMap _tcp_procs;
	_ServerMap _servers;
	_ReqBufPortMap _request_buffer;
	unsigned int _timeout_ms;
	int _backlog_size;
	async::Logger &_logger;

	void parseMaxBodySize(const ConfigContext &root_context);
	void parseUploadStore(const ConfigContext &root_context);
	void parseTimeout(const ConfigContext &root_context);
	void parseBacklogSize(const ConfigContext &root_context);
	void parseServer(const ConfigContext &server_context);

	void parseRequestForEachFd(int port, async::TCPIOProcessor &tcp_proc);
	_Servers::iterator findNoneNameServer(int port);
	HTTP::Request &getRequestBuffer(int port, int client_fd);
	void resetRequestBuffer(int port, int client_fd);
	void registerRequest(int port, int client_fd, HTTP::Request &request);
	void retrieveResponseForEachFd(int port, _Servers &servers);
	HTTP::Response generateErrorResponse(const int code);
	void disconnect(int port, int client_fd);
	void terminate(void);

  public:
	WebServer(const ConfigContext &root_context);
	~WebServer();

	int task(void);
	static void setTerminationFlag(void);
};

#endif
