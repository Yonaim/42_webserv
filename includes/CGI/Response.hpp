#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "HTTP/Response.hpp"
#include "Header.hpp"
#include <string>
#include <vector>

namespace CGI
{
/**
 * @class Response
 * @brief CGI 응답의 상태 코드, 헤더 필드, 메시지 본문을 저장하는 클래스
 *
 * Response는 CGI 실행 결과를 파싱하여 CGI 응답의 상태코드, 헤더 필드, 메시지
 * 본문을 각각 파싱하여 적절히 저장한다.
 * 또한 toHTTPResponse 멤버 함수를 통해 CGI 응답을 HTTP 응답으로 변환해서 반환한다.
 */

class Response
{
  private:
	Header _header;
	int _status_code;
	std::string _response_body;

  public:
	Response();
	~Response();
	Response(const Response &orig);
	const Response &operator=(const Response &orig);

	void makeResponse(std::string &cgi_output);
	void consumeHeader(std::string &buffer);

	HTTP::Response toHTTPResponse(void) const;
};
} // namespace CGI

#endif