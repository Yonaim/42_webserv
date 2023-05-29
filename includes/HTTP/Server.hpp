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
	std::set<std::string> _cgi_extensions; // 보너스 대비하여 set로
	std::map<int, std::queue<RequestHandler *> > _request_handlers;
	std::map<int, std::queue<CGI::RequestHandler *> > _cgi_handlers;
	std::map<int, std::queue<Response> > _output_queue;
	size_t _max_body_size;
	const unsigned int _timeout_ms;
	async::Logger &_logger;

	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);
	void parseDirectiveCGI(const ConfigContext &server_context);
	void ensureClientConnected(int client_fd);
	static bool isValidStatusCode(const int &status_code);
	std::string getErrorPage(const int code);
	Response generateErrorResponse(const int code);
	void registerErrorResponse(const int fd, const int code);
	void registerRedirectResponse(const int fd,
								  const Server::Location &location);

  public:
	class LocationNotFound;
	class ClientNotFound;

	Server(const ConfigContext &server_context, const size_t max_body_size,
		   const unsigned int timeout_ms);
	~Server();
	Server(const Server &orig);
	Server &operator=(const Server &orig);

	void task(void);
	bool isForMe(const Request &request);
	void registerCGIRequest(int client_fd, const Request &request,
							const std::string &resource_path);
	void registerHTTPRequest(int client_fd, const Request &request,
							 const Location &location,
							 const std::string &resource_path);
	void registerRequest(int client_fd, const Request &request);
	Response retrieveResponse(int client_fd);
	int hasResponses(void);
	bool hasResponses(int client_fd);
	bool hasServerName(void) const;
	void disconnect(int client_fd);

	int getPort(void) const;
	unsigned int getTimeout(void) const;
	bool cgiEnabled(void) const;
	const Location &getLocation(const std::string &location) const;
	bool isCGIextension(const std::string &path) const;

	void iterateRequestHandlers(void);
	void iterateCGIHandlers(void);
};
} // namespace HTTP

#include "HTTP/ServerExceptions.hpp"
#include "HTTP/ServerLocation.hpp"
#include "RequestHandler.hpp"

#endif
