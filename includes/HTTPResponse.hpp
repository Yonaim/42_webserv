#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HTTPHeader.hpp"

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

/**
 * TODO:
 * 	1. chunked-encoding 처리
 *  2. 특정 헤더 필드 처리(RFC 문서 참고하면서)
 *  3. 테스트 및 컴파일 잘되는지 확인하기
 */

namespace HTTP
{

class Response
{
  public:
	Response();
	Response(Response const &other);
	Response &operator=(Response const &other);
	~Response();

	std::string toString(void);

	// setter
	void setDate(void);
	void setDefault(void);
	void setStatus(int status_code);
	void setConnection(void);
	void setContentLength(void);
	void setValue(std::string const &key, std::string const &val);

  private:
	typedef std::map<std::string, std::string>::iterator _header_iterator;

	void initGeneralHeaderFields(void);
	void initResponseHeaderFields(void);
	void initEntityHeaderFields(void);
	void makeStatusLine(void);
	void makeHeader(void);
	void makeBody(void);
	bool isComplete(void) const;

	// final
	std::string _response;

	// status-line
	static const std::string _http_version;
	std::string _status_code;
	std::string _reason_phrase;
	Header _header;
	std::string _body;
};
} // namespace HTTP

#endif
