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
	HttpReq(std::string const &req);

	bool parse();

	int                           getMethod() const;
	std::string const            &getUri() const;
	std::string const            &getVersion() const;
	str_vec_map_t const          &getHeader() const;
	str_vec_map_t::const_iterator getHeaderVal(std::string const &key) const;
	std::string const            &getBody() const;
	bool                          isChunked() const;
	void                          appendChunk(std::string const &chunk);
	bool hasHeaderVal(std::string const &key, std::string const &val) const;

  private:
	std::string const &_src;
	size_t             _offset;
	int                _method; // 메서드 : GET, POST, DELETE
	std::string        _uri;
	std::string        _version;
	str_vec_map_t      _header;
	std::string        _body;
	int                _status;
	bool               _isChunked;

	void                          parseStartLine();
	void                          parseHeader();
	void                          parseBody();
	void                          checkHost();
	void                          checkChunk();
	str_vec_map_t::iterator       findKey(std::string key);
	str_vec_map_t::const_iterator findKey(std::string key) const;
};
} // namespace http_msg

#endif
