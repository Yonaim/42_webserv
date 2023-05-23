#include "CGI/Request.hpp"

CGI::Request::Request()
{
}

CGI::Request::~Request()
{
}

CGI::Request::Request(const HTTP::Request &http_request)
{
	(void)http_request;
	// TODO: http request를 인자로 받는 생성자 구현
}

CGI::Request::Request(const Request &orig)
{
	(void)orig;
	// TODO: 복사 생성자 구현
}

const CGI::Request &CGI::Request::operator=(const Request &orig)
{
	(void)orig;
	// TODO: 복사 할당 연산자 구현
	return (*this);
}
