#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "HTTP/Request.hpp"

namespace CGI
{
class Request
{
  private:
	static const std::string _version;
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
	void setMetaVariable(const std::string &name, const std::string &value);
	void setValues(const HTTP::Request &http_req,
				   const std::string &resource_path);

	// getter
	const std::string &getMessageBody(void) const;
	char *const *getEnv(void) const;
};
} // namespace CGI

#endif