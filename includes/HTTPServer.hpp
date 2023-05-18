#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "AsyncTCPIOProcessor.hpp"
#include "ConfigDirective.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include <queue>
#include <set>
#include <string>
#include <vector>

enum e_http_method
{
	GET = 0,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

namespace HTTP
{
class Server
{
  private:
	class Location
	{
	  private:
		bool _has_index;
		bool _do_redirection;
		bool _autoindex;
		std::string _path;
		std::string _root;
		std::vector<std::string> _index;
		std::pair<int, std::string> _redirection;
		std::set<int> _allowed_methods;

	  public:
		Location();
		Location(const ConfigContext &location_context);
		~Location();
		Location(const Location &orig);
		Location &operator=(const Location &orig);

		void parseDirectiveRoot(const ConfigContext &location_context);
		void parseDirectiveLimitExcept(const ConfigContext &location_context);
		void parseDirectiveReturn(const ConfigContext &location_context);
		void parseDirectiveAutoIndex(const ConfigContext &location_context);
		void parseDirectiveIndex(const ConfigContext &location_context);
		const std::string &getPath(void);
	};

	int _port;
	std::set<std::string> _server_name;
	std::map<int, std::string> _error_pages;
	std::map<std::string, Location> _locations;
	std::queue<Request> _input_queue;
	std::queue<Response> _output_queue;
	static const std::map<std::string, int> _http_methods;
	static const std::map<int, std::string> _http_status_code;

	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);
	static bool isValidStatusCode(const int &status_code);

  public:
	Server(const ConfigContext &server_context);
	~Server();
	Server(const Server &orig);
	Server &operator=(const Server &orig);

	void task(void);
	bool isForMe(const Request &request);
	void registerRequest(const Request &request);
	Response retrieveResponse(void);
	bool hasResponses(void);
};
} // namespace HTTP

#endif
