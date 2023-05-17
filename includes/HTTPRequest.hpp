#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HTTPHeader.hpp"
#include <map>
#include <string>
#include <vector>

namespace HTTP
{
// special token
const std::string CRLF = "\r\n";
const std::string SP = " ";
const int CRLF_LEN = 2;
const int SP_LEN = 1;

const int kMethodCount = 4;
const std::string kMethodStr[kMethodCount] = {"NONE", "GET", "POST", "DELETE"};

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
	enum method_type_e
	{
		METHOD_NONE = 0,
		METHOD_GET,
		METHOD_POST,
		METHOD_DELETE
	};

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
	int _version; // 1001 2000 1000
	Header _header;
	std::string _body;
	int _current_state; // enum parse_state_e
	size_t _error_offset; // 에러가 발생한 위치
	std::vector<std::string> _trailer_values;

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
	const std::string &getHeaderValue(const std::string &name, int idx) const;
};
} // namespace HTTP

#endif
