#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HTTP/Header.hpp"
#include "async/Logger.hpp"
#include <map>
#include <string>
#include <vector>

namespace HTTP
{
enum consume_exc_e
{
	CONSUME_EXC_EMPTY_LINE = 0,
	CONSUME_EXC_INVALID_FORMAT,
	CONSUME_EXC_INVALID_FIELD,
	CONSUME_EXC_INVALID_VALUE
};

class Request
{
  private:
	enum parse_state_e
	{
		PARSE_STATE_STARTLINE = 0,
		PARSE_STATE_HEADER,
		PARSE_STATE_BODY,
		PARSE_STATE_CHUNK,
		PARSE_STATE_TRAILER,
		PARSE_STATE_CRLF_AFTER_CHUNK
	};

	int _method;
	std::string _uri;
	std::string _version_string; // 1.1 2.0 1.0
	int _version;                // 1001 2000 1000
	Header _header;
	std::string _body;
	int _current_state;   // enum parse_state_e
	size_t _error_offset; // 에러가 발생한 위치
	std::vector<std::string> _trailer_values;
	int _content_length;
	async::Logger &_logger;

	int consumeStartLine(std::string &buffer);
	int consumeHeader(std::string &buffer);
	int consumeBody(std::string &buffer);
	int consumeChunk(std::string &buffer);
	int consumeCRLF(std::string &buffer);
	int consumeTrailer(std::string &buffer);
	void throwException(int code) const;

  public:
	enum return_type_e
	{
		RETURN_TYPE_OK = 0,
		RETURN_TYPE_INVALID,
		RETURN_TYPE_AGAIN,
		RETURN_TYPE_IN_PROCESS
	};
	Request(void);
	~Request();
	Request(const Request &orig);
	Request &operator=(const Request &orig);

	int parse(std::string &buffer);

	bool hasHeaderValue(const Header::const_iterator &name_iter,
						const std::string &value) const;
	bool hasHeaderValue(const std::string &name,
						const std::string &value) const;
	bool hasHeaderValue(const std::string &name) const;

	// getter
	const std::string &getHeaderValue(const std::string &name, int idx) const;
	int getMethod(void) const;
	const std::string &getURIPath(void) const;
	const std::string &getBody() const;
};
} // namespace HTTP

#endif
