#ifndef SERVER_HPP
#define SERVER_HPP

#include "ConfigDirective.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
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
	class ServerException;

	int _port;
	std::set<std::string> _server_name;
	std::map<int, std::string> _error_pages;
	std::map<std::string, Location> _locations;
	std::map<int, std::queue<RequestHandler *> > _request_handlers;
	std::map<int, std::queue<Response> > _output_queue;

	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);
	void ensureClientConnected(int client_fd);
	static bool isValidStatusCode(const int &status_code);

  public:
	Server(const ConfigContext &server_context);
	~Server();
	Server(const Server &orig);
	Server &operator=(const Server &orig);

	int getPort(void) const;
	void task(void);
	bool isForMe(const Request &request);
	void registerRequest(int client_fd, const Request &request);
	Response retrieveResponse(int client_fd);
	int hasResponses(void);
	bool hasResponses(int client_fd);
	void disconnect(int client_fd);

	// 추가
	const Location &getLocation(const std::string &location) const;
	void setErrorPage(HTTP::Response &response, int status_code);
	std::string getResourcePath(const Request &req) const;
};
} // namespace HTTP

#include "HTTP/ServerException.hpp"
#include "HTTP/ServerLocation.hpp"
#include "RequestHandler.hpp"

#endif
