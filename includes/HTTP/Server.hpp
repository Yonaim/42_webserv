#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "CGI/RequestHandler.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "async/FileIOHandler.hpp"
#include "async/Logger.hpp"
#include "async/TCPIOProcessor.hpp"
#include "utils/shared_ptr.hpp"
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

	typedef ft::shared_ptr<async::FileReader> _FileReaderPtr;
	typedef ft::shared_ptr<RequestHandler> _RequestHandlerPtr;
	typedef ft::shared_ptr<CGI::RequestHandler> _CGIRequestHandlerPtr;

	static const int _http_min_version; // should be min <= ver <= max
	static const int _http_max_version; // (note that it is not min < ver < max)
	int _port;
	bool _has_server_name;
	bool _cgi_enabled;
	std::set<std::string> _server_name;
	std::map<int, std::string> _error_page_paths;
	std::map<std::string, _FileReaderPtr> _error_page_readers;
	std::map<std::string, Location> _locations;
	std::map<std::string, std::string> _cgi_ext_to_path;
	std::string _temp_dir_path;
	std::set<int> _allowed_cgi_methods;
	std::map<int, std::queue<_RequestHandlerPtr> > _request_handlers;
	std::map<int, std::queue<_CGIRequestHandlerPtr> > _cgi_handlers;
	std::map<int, std::queue<Response> > _output_queue;
	size_t _max_body_size;
	const unsigned int _timeout_ms;
	async::Logger &_logger;

	static bool isValidStatusCode(const int &status_code);

	// parse directive
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

	// interfaces
	void task(void);
	void registerCGIRequest(int client_fd, const Request &request,
							const std::string &exec_path,
							const std::string &resource_path);
	void registerHTTPRequest(int client_fd, const Request &request,
							 const Location &location,
							 const std::string &resource_path);
	void registerRequest(int client_fd, const Request &request);
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
