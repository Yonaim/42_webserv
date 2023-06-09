#include "HTTP/Request.hpp"
#include "HTTP/ParsingFail.hpp"
#include "HTTP/const_values.hpp"
#include "utils/ansi_escape.h"
#include "utils/string.hpp"

using namespace HTTP;

Request::Request(void)
	: _method(METHOD_NONE), _current_state(PARSE_STATE_STARTLINE),
	  _content_length(0), _logger(async::Logger::getLogger("Request"))
{
}

Request::~Request()
{
}

Request::Request(const Request &orig)
	: _method(orig._method), _uri(orig._uri), _version(orig._version),
	  _header(orig._header), _body(orig._body),
	  _current_state(orig._current_state),
	  _content_length(orig._content_length), _logger(orig._logger)
{
}

Request &Request::operator=(const Request &orig)
{
	if (this != &orig)
	{
		_method = orig._method;
		_uri = orig._uri;
		_version = orig._version;
		_header = orig._header;
		_body = orig._body;
		_current_state = orig._current_state;
		_content_length = orig._content_length;
	}
	return (*this);
}

bool Request::hasHeaderValue(std::string const &name,
							 std::string const &value) const
{
	return (_header.hasValue(name, value));
}

bool Request::hasHeaderValue(const Header::const_iterator &name_iter,
							 const std::string &value) const
{
	return (_header.hasValue(name_iter, value));
}

bool Request::hasHeaderValue(std::string const &name) const
{
	return (_header.hasValue(name));
}

const std::string &Request::getHeaderValue(std::string const &name,
										   int idx) const
{
	return (_header.getValue(name, idx));
}

size_t Request::countHeaderValue(const std::string &name) const
{
	return (_header.countValue(name));
}

int Request::getMethod(void) const
{
	return (_method);
}

const std::string &Request::getMethodString(void) const
{
	return (HTTP::METHOD.getKeyByValue(_method));
}

const int &Request::getVersion(void) const
{
	return (_version_num);
}

const std::string &Request::getVersionString(void) const
{
	return (_version);
}

const std::string &Request::getURIPath(void) const
{
	return (_uri);
}

const std::string &Request::getQueryString(void) const
{
	return (_query_string);
}

const std::string &Request::getBody() const
{
	return (_body);
}

const Header &Request::getHeader(void) const
{
	return (_header);
}

const std::string Request::getDescription(void) const
{
	const size_t bodylen = 20;

	std::stringstream buf;
	buf << ANSI_BBLUE;
	buf << "[" << HTTP::METHOD.getKeyByValue(_method) << " " << _uri << " | ";
	if (_body.size() > bodylen)
		buf << _body.substr(0, bodylen - 3) << "...";
	else
		buf << _body;
	buf << "]" << ANSI_RESET;
	return (buf.str());
}
