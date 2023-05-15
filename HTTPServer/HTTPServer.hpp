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
		std::string path;
		std::string root;
		std::string index;
		std::pair<int, std::string> redirection;
		std::unordered_set<int> allowed_methods;
		static const std::map<std::string, bool> directives_info;

	  public:
		HTTPLocation();
		HTTPLocation(const ConfigContext &location_context);
		~HTTPLocation();
		HTTPLocation(const HTTPLocation &orig);
		HTTPLocation &operator=(const HTTPLocation &orig);
		const std::string &getPath(void);
	};

	int port;
	std::string server_name;
	std::map<int, std::string> error_pages;
	std::map<std::string, HTTPLocation> locations;
	static const std::map<std::string, bool> directives_info;

	void addLocationInfo(const ConfigContext &location_context);
	void addOtherInfo(const ConfigDirective &directive);

  public:
	HTTPServer(const ConfigContext &server_context);
	~HTTPServer();
	HTTPServer(const HTTPServer &orig);
	HTTPServer &operator=(const HTTPServer &orig);
};

#endif
