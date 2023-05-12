#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "../AsyncIOProcessor/AsyncTCPIOProcessor.hpp"
#include "../ConfigParser/ConfigDirective.hpp"
#include <string>
#include <unordered_set>
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
		bool has_index;
		bool do_redirection;
		bool autoindex;
		std::string root;
		std::string index;
		std::pair<int, std::string> redirection;
		std::unordered_set<int> allowed_methods;

	  public:
		HTTPLocation();
		HTTPLocation(const ConfigContext &location_context);
		~HTTPLocation();
		HTTPLocation(const HTTPLocation &orig);
		HTTPLocation &operator=(const HTTPLocation &orig);
	};

	int port;
	std::string server_name;
	std::map<int, std::string> error_pages;
	std::vector<std::pair<std::string, HTTPLocation>> locations;

	void addLocation(const ConfigContext &location_context);
	void addOtherInfo(const ConfigDirective &directive);

  public:
	HTTPServer(const ConfigContext &server_context);
	~HTTPServer();
	HTTPServer(const HTTPServer &orig);
	HTTPServer &operator=(const HTTPServer &orig);
};

#endif
