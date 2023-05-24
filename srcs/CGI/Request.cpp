#include "CGI/Request.hpp"

using namespace CGI;

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(const HTTP::Request &http_request)
{
	(void)http_request;
	// TODO: http request를 인자로 받는 생성자 구현
}

Request::Request(const Request &orig)
{
	(void)orig;
	// TODO: 복사 생성자 구현
}

const Request &Request::operator=(const Request &orig)
{
	(void)orig;
	// TODO: 복사 할당 연산자 구현
	return (*this);
}

const std::string &Request::getMessageBody(void) const
{
	return (_message_body);
}

char *const *Request::getEnv(void) const
{
	// TODO: getEnv 구현
	return (NULL);
}

const std::string Request::getPath() const
{
	// TODO: getPath() 구현
	std::string str;
	return (str);
}