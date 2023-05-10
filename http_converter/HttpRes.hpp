#ifndef HTTP_RES_HPP
#define HTTP_RES_HPP
#include "http_msg_const.hpp"
#include <map>

// 시작줄 : [HTTP 버전] [상태 코드] [사유 구절] # 공백으로 띄워진다.
// 헤더, 빈 줄, 엔티티 본문이 온다.

namespace http_msg
{

class HttpRes
{
  public:
	HttpRes(int status);
	str_t toString(void);

	void setHeaderField(std::string const &key, std::string const &val);
	// server
	// Content-Length
	// Content-Type

  private:
	str_t _dest;

	const str_t                _version = "HTTP/1.1";
	int                        _status;
	str_t                      _comment;
	str_t                      _body; // 실제 엔티티 본문
	std::map<str_t, str_vec_t> _header;
};
} // namespace http_msg

#endif
