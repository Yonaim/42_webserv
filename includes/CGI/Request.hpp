#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "HTTP/Request.hpp"
#include "async/Logger.hpp"

namespace CGI
{
class Request
{
  private:
	static const std::string _version;
	std::map<std::string, std::string> _meta_variables;
	std::string _message_body;
	async::Logger &_logger;

  public:
	Request(const HTTP::Request &http_req, const std::string &resource_path);
	~Request();
	Request(const Request &orig);
	const Request &operator=(const Request &orig);

	// setter
	void setMetaVariable(const std::string &name, const std::string &value);
	void setMessageBody(std::string message_body);

	// getter
	char *const *getEnv(void) const;
	const std::string &getPath(void) const;
	const std::string &getMessageBody(void) const;
};
} // namespace CGI

#endif