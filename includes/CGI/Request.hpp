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
	Request(const Request &orig);
	const Request &operator=(const Request &orig);

	// 생성자에서 사용할 파싱 메서드 추가 예정
	const std::string getPath(void) const;
	// setter
	void setMetaVariable(std::string name, std::string value);

	// getter
	const std::string &getMessageBody(void) const;
	char *const *getEnv(void) const;
};
} // namespace CGI

#endif