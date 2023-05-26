#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Header.hpp"
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
	CONSUME_EXC_INVALID_VALUE,
	CONSUME_EXC_INVALID_SIZE,

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
		PARSE_STATE_TRAILER
	};

	int _method;
	std::string _method_string;
	std::string _uri;
	std::string _version_string; // 1.1 2.0 1.0
	int _version;                // 1001 2000 1000
	Header _header;
	std::string _body;
	int _current_state; // enum parse_state_e
	std::vector<std::string> _trailer_values;
	size_t _content_length;
	async::Logger &_logger;

	int consumeStartLine(std::string &buffer);
	int consumeHeader(std::string &buffer);
	int consumeBody(std::string &buffer);
	int consumeChunk(std::string &buffer);
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
	class ParsingFail : public std::runtime_error
	{
	  public:
		ParsingFail(const std::string &why);
	};

	Request(void);
	~Request();
	Request(const Request &orig);
	Request &operator=(const Request &orig);

	int parse(std::string &buffer, size_t client_max_body_size);

	bool hasHeaderValue(const Header::const_iterator &name_iter,
						const std::string &value) const;
	bool hasHeaderValue(const std::string &name,
						const std::string &value) const;
	bool hasHeaderValue(const std::string &name) const;

	// getter
	const std::string &getHeaderValue(const std::string &name, int idx) const;
	int getMethod(void) const;
	const std::string &getMethodString(void) const;
	const std::string &getURIPath(void) const;
	const std::string &getBody() const;
};
} // namespace HTTP

#endif
