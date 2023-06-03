#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "ConfigDirective.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/Logger.hpp"
#include "async/TCPIOProcessor.hpp"
#include <map>
#include <string>
#include <vector>

/**
 * @brief 웹서버 동작에 필요한 모든 것을 담고 있는 클래스.
 * 하나의 port에 여러 개의 server가 있을 수 있기에 port를 key로 하는 map을 가진다.
 */
class WebServer
{
  private:
	typedef std::map<int, async::TCPIOProcessor *> _TCPProcMap;
	typedef std::vector<HTTP::Server> _Servers;
	typedef std::map<int, _Servers> _ServerMap;
	typedef std::map<int, HTTP::Request> _ReqBufFdMap;
	typedef std::map<int, _ReqBufFdMap> _ReqBufPortMap;

	static bool _terminate;

	size_t _max_body_size;     // 지정된 최대 body 크기
	std::string _upload_store; // 파일이 upload되는 경로
	_TCPProcMap _tcp_procs;    // port별 tcpIO 처리 객체
	_ServerMap _servers;       // port별 서버 객체 벡터
	_ReqBufPortMap _request_buffer; // 아직 전부 도착하지 않은 request를 port 및
									// client_fd별로 저장하는 버퍼
	unsigned int _timeout_ms; // 지정된 timeout
	int _backlog_size;        // 지정된 backlog 크기
	async::Logger &_logger;

	WebServer(void);

	// parse configuration file
	void parseMaxBodySize(const ConfigContext &root_context);
	void parseUploadStore(const ConfigContext &root_context);
	void parseTimeout(const ConfigContext &root_context);
	void parseBacklogSize(const ConfigContext &root_context);
	void parseServer(const ConfigContext &server_context);

	// methods
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
	WebServer(const WebServer &orig);
	WebServer &operator=(const WebServer &orig);

	int task(void);
	static void setTerminationFlag(void);
};

#endif
