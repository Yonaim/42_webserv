#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

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

  public:
	Response();
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void makeResponse(const std::string &cgi_output);
	void setError(const int status_code);

	// getter
	int getType() const;
	std::string getContentType() const;
	std::string getLocation() const;
	int getStatus() const;
};
} // namespace CGI

#endif