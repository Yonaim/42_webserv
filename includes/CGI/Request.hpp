#ifndef CGI_REQUEST_HPP
#define CGI_REQUEST_HPP

#include "HTTP/Request.hpp"

namespace CGI
{
class Request
{
  private:
	// meta-variables
	// TODO: map으로 전환 여부 고려하기
	std::string _auth_type;
	std::string _content_length;
	std::string _content_type;
	std::string _gateway_interface;
	std::string _path_info;
	std::string _path_translated;
	std::string _query_string;
	std::string _remote_addr;
	std::string _remote_host;
	std::string _remote_ident;
	std::string _remote_user;
	std::string _request_method;
	std::string _script_name;
	std::string _server_name;
	std::string _server_port;
	std::string _server_protocol;
	std::string _server_software;
	std::vector<std::string> _extra_variables;
	// message-body
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
