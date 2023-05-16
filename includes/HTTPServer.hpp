#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "AsyncTCPIOProcessor.hpp"
#include "ConfigDirective.hpp"
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

class HTTPServer
{
  private:
	class HTTPLocation
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
		HTTPLocation();
		HTTPLocation(const ConfigContext &location_context);
		~HTTPLocation();
		HTTPLocation(const HTTPLocation &orig);
		HTTPLocation &operator=(const HTTPLocation &orig);

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
	std::map<std::string, HTTPLocation> _locations;
	static const std::map<std::string, int> _http_methods;
	static const std::map<int, std::string> _http_status_code;

	void parseDirectiveListen(const ConfigContext &server_context);
	void parseDirectiveErrorPage(const ConfigContext &server_context);
	void parseDirectiveServerName(const ConfigContext &server_context);
	void parseDirectiveLocation(const ConfigContext &server_context);

  public:
	HTTPServer(const ConfigContext &server_context);
	~HTTPServer();
	HTTPServer(const HTTPServer &orig);
	HTTPServer &operator=(const HTTPServer &orig);
};

#endif
