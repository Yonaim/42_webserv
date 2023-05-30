#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "Header.hpp"
#include "HTTP/Response.hpp"
#include <string>
#include <vector>

namespace CGI
{
enum consume_exc_e
{
	CONSUME_EXC_EMPTY_LINE = 0,
	CONSUME_EXC_INVALID_FORMAT,
	CONSUME_EXC_INVALID_FIELD,
	CONSUME_EXC_INVALID_VALUE,
	CONSUME_EXC_INVALID_SIZE,
};

class Response
{
  private:
	Header _header;
	int _status_code;
	std::string _response_body;

  public:
	class ParsingFail : public std::runtime_error
	{
	  public:
		ParsingFail(const std::string &why);
	};

	Response();
	Response(std::string &cgi_output);
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void setError(const int status_code);
	void consumeHeader(std::string &buffer);
	void throwException(int code) const;

	HTTP::Response toHTTPResponse(void) const;
};
} // namespace CGI

#endif