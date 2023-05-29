#include "CGI/Response.hpp"
#include "CGI/const_values.hpp"
#include "utils/string.hpp"

CGI::Response::Response()
{
}

CGI::Response::~Response()
{
}

CGI::Response::Response(const Response &orig)
{
	(void)orig;
	// TODO: 복사 생성자 구현
}

const CGI::Response &CGI::Response::operator=(const Response &orig)
{
	(void)orig;
	// TODO: 복사 할당 연산자 구현
	return (*this);
}

void CGI::Response::makeResponse(std::string &cgi_output)
{
	// TODO: string으로 받은 Response 메시지 기반으로 CGI/Response 초기화

	while (cgi_output.substr(0, NL_LEN) != NL)
	{
		parseHeader(cgi_output);
	}
	for (Header::iterator it = _header.begin(); it != _header.end(); it++)
	{

	}

	// parsing content-type
	// parsing status (option)
	// skip NL
	// parsing reponse-body
}

void CGI::Response::parseHeader(std::string &line)
{
	const size_t nl_pos = line.find(NL);
	if (nl_pos == std::string::npos)
		throw(std::runtime_error("hello"));

	const std::string header_line = consumestr(line, nl_pos);
	consumestr(line, NL_LEN);

	if (nl_pos == 0) // CRLF만 있는 줄: 헤더의 끝을 의미
		throw(std::runtime_error(""));

	const size_t colon_pos = header_line.find(":");
	if (colon_pos == std::string::npos)
		throw(std::runtime_error(""));

	const std::string name = getfrontstr(header_line, colon_pos);
	if (hasSpace(name))
		throw(std::runtime_error(""));

	const std::string value_part = getbackstr(header_line, colon_pos + 1);
	std::vector<std::string> values = split(value_part, ",");
	for (std::vector<std::string>::iterator it = values.begin();
		 it != values.end(); it++)
		strtrim(*it, LWS);

	/* key가 있다면, 붙여넣기 */
	if (!_header.hasValue(name))
		_header.assign(name, values);
	else
		_header.insert(name, values);
}

void CGI::Response::setError(const int status_code)
{
	_status = status_code;
}

std::string CGI::Response::getContentType() const
{
	return (_content_type);
}

int CGI::Response::getStatus() const
{
	return (_status);
}