#include "CGI/Response.hpp"
#include "CGI/const_values.hpp"
#include "HTTP/ParsingFail.hpp"
#include "HTTP/Response.hpp"
#include "utils/string.hpp"

using namespace CGI;

CGI::Response::Response()
{
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

void CGI::Response::makeResponse(std::string &cgi_output)
{
	while (cgi_output.substr(0, CRLF_LEN) != CRLF)
		consumeHeader(cgi_output);
	consumestr(cgi_output, CRLF_LEN);

	if (!_header.hasValue("Content-Type"))
		throw(HTTP::InvalidFormat());

	if (_header.hasValue("Status"))
	{
		const std::vector<std::string> values = _header.getValues("Status");
		if (values.size() != 2)
			throw(HTTP::InvalidFormat());
		_status_code = toNum<int>(values[0]);
	}
	else
		_status_code = 200;

	_response_body = cgi_output;
}

void CGI::Response::consumeHeader(std::string &buffer)
{
	const size_t nl_pos = buffer.find(CRLF);
	if (nl_pos == std::string::npos)
		throw(HTTP::InvalidFormat());

	const std::string header_line = consumestr(buffer, nl_pos);
	consumestr(buffer, CRLF_LEN);

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
		throw(HTTP::InvalidField());
	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
		throw(HTTP::InvalidField());

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

HTTP::Response CGI::Response::toHTTPResponse(void) const
{
	HTTP::Response http_response(_header);

	http_response.setBody(_response_body);
	http_response.setContentLength(_response_body.length());
	http_response.setStatus(_status_code);
	return (http_response);
}
