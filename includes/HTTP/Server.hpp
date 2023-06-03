#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "CGI/RequestHandler.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/Logger.hpp"
#include "async/TCPIOProcessor.hpp"
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace HTTP
{
class Server
{
  public:
	class RequestHandler;
	class RequestGetHandler;
	class RequestHeadHandler;
	class RequestPostHandler;
	class RequestPutHandler;
	class RequestDeleteHandler;

  private:
	class Location;

	int _port;
	bool _has_server_name;
	bool _cgi_enabled;
	std::set<std::string> _server_name;
	std::map<int, async::FileReader *> _error_pages;
	std::map<std::string, Location> _locations;
	std::string _cgi_extension; // TODO: std::set에 넣기
	std::string _cgi_exec_path;
	std::string _temp_dir_path;
	std::set<int> _allowed_cgi_methods;
	std::map<int, std::queue<RequestHandler *> > _request_handlers;
	std::map<int, std::queue<CGI::RequestHandler *> > _cgi_handlers;
	std::map<int, std::queue<Response> > _output_queue;
	size_t _max_body_size;
	const unsigned int _timeout_ms;
	async::Logger &_logger;

	static bool isValidStatusCode(const int &status_code);

	// parse configuration file
	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);
	void parseDirectiveCGI(const ConfigContext &server_context);
	void parseDirectiveCGILimitExcept(const ConfigContext &server_context);
	void parseDirectiveTmpDirPath(const ConfigContext &server_context);

	// utils of interfaces
	Response generateErrorResponse(const int code);
	void iterateRequestHandlers(void);
	void iterateCGIHandlers(void);

  public:
	class ServerError;
	class LocationNotFound;
	class ClientNotFound;
	class InvalidRequest;

	Server(const ConfigContext &server_context, const size_t max_body_size,
		   const unsigned int timeout_ms);
	~Server();
	Server(const Server &orig);
	Server &operator=(const Server &orig);

	// interfaces
	void task(void);
	void registerRequest(int client_fd, const Request &request);
	void registerHTTPRequest(int client_fd, const Request &request,
							 const Location &location,
							 const std::string &resource_path);
	void registerCGIRequest(int client_fd, const Request &request,
							const std::string &resource_path);
	Response retrieveResponse(int client_fd);
	void registerRedirectResponse(int fd, const Server::Location &location);
	void registerErrorResponse(int fd, int code);
	void disconnect(int client_fd);

	// methods
	void ensureClientConnected(int client_fd) const;
	bool isForMe(const Request &request) const;
	bool hasServerName(void) const;
	bool cgiAllowed(int method) const;
	bool hasResponses(int client_fd) const;
	int hasResponses(void) const;
	bool isCGIextension(const std::string &path) const;
	int getPort(void) const;
	unsigned int getTimeout(void) const;
	std::string getErrorPage(int code);
	const Location &getLocation(const std::string &location) const;
};
} // namespace HTTP

#include "HTTP/ServerError.hpp"
#include "HTTP/ServerLocation.hpp"
#include "RequestHandler.hpp"

#endif
