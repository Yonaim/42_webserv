#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

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
	class RequestDeleteHandler;

  private:
	class Location;

	int _port;
	bool _has_server_name;
	std::set<std::string> _server_name;
	std::string _root;
	std::map<int, async::FileReader *> _error_pages;
	std::map<std::string, Location> _locations;
	std::map<int, std::queue<RequestHandler *> > _request_handlers;
	std::map<int, std::queue<Response> > _output_queue;
	async::Logger &_logger;

	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveRoot(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);
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

	Server(const ConfigContext &server_context);
	~Server();
	Server(const Server &orig);
	Server &operator=(const Server &orig);

	void task(void);
	bool isForMe(const Request &request);
	void registerRequest(int client_fd, const Request &request);
	Response retrieveResponse(int client_fd);
	int hasResponses(void);
	bool hasResponses(int client_fd);
	bool hasServerName(void) const;
	void disconnect(int client_fd);

	int getPort(void) const;
	const Location &getLocation(const std::string &location) const;
	std::string getResourcePath(const Request &req) const;
};
} // namespace HTTP

#include "HTTP/ServerExceptions.hpp"
#include "HTTP/ServerLocation.hpp"
#include "RequestHandler.hpp"

#endif
