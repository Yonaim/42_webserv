#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "Header.hpp"
#include "async/Logger.hpp"
#include <map>
#include <string>
#include <vector>

namespace HTTP
{
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
	std::string _uri;
	std::string _query_string;
	std::string _version; // 1.1 2.0 1.0
	Header _header;
	std::string _body;
	int _current_state; // enum parse_state_e
	std::vector<std::string> _trailer_values;
	size_t _content_length;
	size_t _max_body_size;
	async::Logger &_logger;

	int parseStartLine(std::string &buffer);
	int parseHeader(std::string &buffer);

	int consumeStartLine(std::string &buffer);
	int consumeHeader(std::string &buffer);
	int consumeBody(std::string &buffer);
	int consumeChunk(std::string &buffer);
	int consumeTrailer(std::string &buffer);

  public:
	enum return_type_e
	{
		RETURN_TYPE_OK = 0,
		RETURN_TYPE_INVALID,
		RETURN_TYPE_AGAIN,
		RETURN_TYPE_IN_PROCESS
	};

	Request(size_t max_body_size);
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
	size_t countHeaderValue(const std::string &name) const;
	int getMethod(void) const;
	const std::string &getMethodString(void) const;
	const std::string &getURIPath(void) const;
	const std::string &getQueryString(void) const;
	const std::string &getBody(void) const;
	const Header &getHeader(void) const;
	const std::string getDescription(void) const;
};
} // namespace HTTP

inline async::Logger &operator<<(async::Logger &io, HTTP::Request &content)
{
	if (!io.isActive())
		return (io);
	io.log(content.getDescription());
	return (io);
}

#endif
