#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include <string>
#include <vector>

namespace CGI
{
class Response
{
  private:
	int _type;
	std::string _content_type;
	int _status;
	std::string _response_body;

  public:
	// enum _response_type_e
	// {
	// 	RESPONSE_DOCUMENT,
	// 	RESPONSE_LOCAL_REDIR,
	// 	RESPONSE_CLIENT_REDIR,
	// 	RESPONSE_CLIENT_REDIRDOC
	// };
	Response();
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void makeResponse(const std::string &cgi_output);
	void setError(int status_code);

	// getter
	int getType() const;
	std::string getContentType() const;
	std::string getLocation() const;
	int getStatus() const;
};
} // namespace CGI

#endif