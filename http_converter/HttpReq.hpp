#ifndef HTTP_REQ_HPP
#define HTTP_REQ_HPP

#include "http_msg.hpp"

// 시작줄 : [메서드] [URI] [HTTP 버전] # 공백으로 띄워진다.
// 헤더, 빈 줄, 엔티티 본문이 온다.

namespace http_msg
{
class HTTPReq
{
  public:
	HTTPReq(std::string const &req);
	// TODO: Trailer 헤더가 있는 경우 Trailer Type의 HTTPReq가 있는지 확인할 것
	// - ALL이면 사이즈가 1
	// - 그게 아니면 HEAD CHUNK [*CHUNK] [TRAILER]
	HTTPReq(std::queue<HTTPReq> const &chunks);

	// getter
	int                           getType() const;
	int                           getMethod() const;
	std::string const            &getUri() const;
	std::string const            &getVersion() const;
	str_vec_map_t const          &getHeader() const;
	str_vec_map_t::const_iterator getHeaderVal(std::string const &key) const;
	std::string const            &getBody() const;

	bool hasHeaderVal(std::string const &key, std::string const &val) const;

  private:
	enum eType
	{
		NONE = -1,
		ALL,
		HEAD,
		CHUNK,
		TRAILER
	};

	enum eMethodType
	{
		GET,
		POST,
		DELETE
	};

	std::string const &_src;
	size_t             _offset;
	int                _type;
	int                _method; // 메서드 : GET, POST, DELETE
	std::string        _uri;
	std::string        _version;
	str_vec_map_t      _header;
	std::string        _body;

	void parseStartLine();
	void parseHeader();
	void checkHeader();
	void parseBody();
	void parseChunk();
	void checkHost();
	bool isChunkType();
	bool hasChunkedVal();
};
} // namespace http_msg

#endif
