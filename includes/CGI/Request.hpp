#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "HTTP/Request.hpp"
#include "async/Logger.hpp"

namespace CGI
{

/**
 * @class Request
 * @brief CGI 요청을 처리하는 클래스
 *
 * Request는 CGI 요청(request)을 처리하기 위한 클래스이다. HTTP::Request와 리소스
 * 경로를 입력으로 받아 CGI 요청에 필요한 CGI 환경 변수(meta-variable)와 메시지
 * 본문을 적절히 저장한다. getEnv 함수와 getPath 함수를 통해 execve의 인자로
 * 넘거야 하는 정보를 얻을 수 있다.
 */
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

	// getter
	char *const *getEnv(void) const;
	const std::string &getPath(void) const;
	const std::string &getMessageBody(void) const;

	void addMetaVariable(const std::string &name, const std::string &value);
	bool isProtocolSpecificHeader(const std::string &name) const;
	std::string toHTTPvarname(const std::string &name) const;
};
} // namespace CGI

#endif