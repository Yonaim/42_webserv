#ifndef HTTP_REQ_HPP
#define HTTP_REQ_HPP

#include "http_msg.hpp"

// 시작줄 : [메서드] [URI] [HTTP 버전] # 공백으로 띄워진다.
// 헤더, 빈 줄, 엔티티 본문이 온다.

namespace http_msg
{
class HttpReq
{
  public:
	HttpReq(str_t const &req);

	bool parse();

	int                       getMethod() const;
	str_t const              &getUri() const;
	str_t const              &getVersion() const;
	str_map_t const          &getHeader() const;
	str_map_t::const_iterator getHeaderVal(str_t const &key) const;
	str_t const              &getBody() const;

  private:
	str_t const &_src;
	size_t       _offset;

	int       _method; // 메서드 : GET, POST, DELETE
	str_t     _uri;
	str_t     _version;
	str_map_t _header;
	str_t     _body;
	int       _status;
	bool      _isChunked;

	void                parseStartLine();
	void                parseHeader();
	void                parseBody();
	void                checkHost();
	void                checkChunk();
	str_map_t::iterator findKey(std::string key);
};
} // namespace http_msg

#endif
