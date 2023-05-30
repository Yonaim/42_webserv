#include "CGI/Response.hpp"
#include "CGI/const_values.hpp"
#include "HTTP/Response.hpp"
#include "utils/string.hpp"

using namespace CGI;

static const char *consume_exc_description[]
	= {"Empty line found", "Invalid format", "Invalid header field",
	   "Invalid header value"};

CGI::Response::ParsingFail::ParsingFail(const std::string &why)
	: std::runtime_error(why)
{
}

CGI::Response::Response()
{
}

CGI::Response::Response(std::string &cgi_output)
{
	while (cgi_output.substr(0, NL_LEN) != NL)
		consumeHeader(cgi_output);
	consumestr(cgi_output, NL_LEN);

	if (!_header.hasValue("Content-Type"))
		throwException(CONSUME_EXC_INVALID_FORMAT);

	if (_header.hasValue("Status"))
	{
		const std::vector<std::string> values = _header.getValues("Status");
		if (values.size() != 2)
			throwException(CONSUME_EXC_INVALID_FORMAT);
		_status_code = toNum<int>(values[0]);
	}
	else
		_status_code = 200;

	_response_body = cgi_output;
}

CGI::Response::~Response()
{
}

CGI::Response::Response(const Response &orig)
	: _header(orig._header), _status_code(orig._status_code),
	  _response_body(orig._response_body)
{
}

const CGI::Response &CGI::Response::operator=(const Response &orig)
{
	if (this != &orig)
	{
		_header = orig._header;
		_status_code = orig._status_code;
		_response_body = orig._response_body;
	}
	return (*this);
}

void CGI::Response::consumeHeader(std::string &buffer)
{
	const size_t nl_pos = buffer.find(NL);
	if (nl_pos == std::string::npos)
		throwException(CONSUME_EXC_INVALID_FORMAT);

	const std::string header_line = consumestr(buffer, nl_pos);
	consumestr(buffer, NL_LEN);

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
		throwException(CONSUME_EXC_INVALID_FIELD);
	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
		throwException(CONSUME_EXC_INVALID_FIELD);

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, " ");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
		strtrim(*it, " \t");

	if (!_header.hasValue(name))
		_header.assign(name, values);
	else
		_header.insert(name, values);
}

void CGI::Response::setError(const int status_code)
{
	_status_code = status_code;
}

HTTP::Response CGI::Response::toHTTPResponse(void) const
{
	HTTP::Response http_response(_header);

	http_response.setBody(_response_body);
	http_response.setContentLength(_response_body.length());
	http_response.setStatus(_status_code);
	return (http_response);
}

void CGI::Response::throwException(int code) const
{
	std::stringstream what;

	if (code < 0 || code > CONSUME_EXC_INVALID_SIZE)
		what << "Unknown error";
	else
		what << consume_exc_description[code];
	throw(ParsingFail(what.str()));
}
