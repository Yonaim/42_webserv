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
	std::string _location;
	int _status;
	std::vector<std::string> _protocol_specific_header_fields;
	std::vector<std::string> _extension_header_fields;
	std::string _message_body;

  public:
	enum _response_type_e
	{
		RESPONSE_DOCUMENT,
		RESPONSE_LOCAL_REDIR,
		RESPONSE_CLIENT_REDIR,
		RESPONSE_CLIENT_REDIRDOC
	};
	Response();
	~Response();
	Response(const std::string &cgi_output);
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	// getter
	int getType() const;
	std::string getContentType() const;
	std::string getLocation() const;
	int getStatus() const;
};
} // namespace CGI

#endif
