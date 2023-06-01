#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "HTTP/Response.hpp"
#include "Header.hpp"
#include <string>
#include <vector>

namespace CGI
{
class Response
{
  private:
	Header _header;
	int _status_code;
	std::string _response_body;

  public:
	Response();
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void makeResponse(std::string &cgi_output);
	void consumeHeader(std::string &buffer);

	HTTP::Response toHTTPResponse(void) const;
};
} // namespace CGI

#endif