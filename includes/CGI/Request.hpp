#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "HTTP/Request.hpp"

namespace CGI
{
class Request
{
  private:
	static const size_t _n_meta_variables = 17;
	std::map<std::string, std::string> _meta_variables;
	std::string _message_body;

  public:
	Request();
	~Request();
	Request(const HTTP::Request &http_request);
	Request(const Request &orig);
	const Request &operator=(const Request &orig);

	// 생성자에서 사용할 파싱 메서드 추가 예정
	const std::string &getMessageBody(void) const;
	const char **getEnv(void) const;
};
} // namespace CGI

#endif
