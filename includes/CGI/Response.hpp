#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "Header.hpp"
#include <string>
#include <vector>

namespace CGI
{
class Response
{
  private:
	std::string _content_type;
	int _status;
	std::string _response_body;
	Header _header;

  public:
	Response();
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void makeResponse(std::string &cgi_output);
	void setError(const int status_code);
	void parseHeader(std::string &line);

	// getter
	int getType() const;
	std::string getContentType() const;
	std::string getLocation() const;
	int getStatus() const;
};
} // namespace CGI

#endif