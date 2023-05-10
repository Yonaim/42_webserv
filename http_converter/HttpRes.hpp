#ifndef HTTP_RES_HPP
#define HTTP_RES_HPP
#include "http_msg_const.hpp"
#include <map>

// 시작줄 : [HTTP 버전] [상태 코드] [사유 구절] # 공백으로 띄워진다.
// 헤더, 빈 줄, 엔티티 본문이 온다.
/*
	HTTP/1.1 200 OK
	Server: nginx/1.23.3
	Date: Wed, 10 May 2023 07:35:44 GMT
	Content-Type: text/html
	Content-Length: 611
	Last-Modified: Wed, 03 May 2023 07:10:36 GMT
	Connection: keep-alive
	ETag: "645208ec-263"
	Accept-Ranges: bytes
*/

namespace http_msg
{

class HttpRes
{
  public:
	HttpRes(int status);
	str_t toString(void);

	void setHeaderField(std::string const &key, std::string const &val);
	// Server
	// Date
	// Content-Type
	// Content-Length
	// Last-Modified
	// Connection
	// ETag
	// Accept-Ranges

  private:
	str_t _dest;

	const str_t                _version;
	int                        _status;
	str_t                      _comment;
	str_t                      _body; // 실제 엔티티 본문
	std::map<str_t, str_vec_t> _header;
};
} // namespace http_msg

#endif
