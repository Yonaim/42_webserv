#include "CGI/Response.hpp"

CGI::Response::Response()
{
}

CGI::Response::~Response()
{
}

CGI::Response::Response(const std::string &cgi_output)
{
	(void)cgi_output;
	// TODO: cgi output을 인자로 받는 생성자 구현
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

int CGI::Response::getType() const
{
	return (_type);
}

std::string CGI::Response::getContentType() const
{
	return (_content_type);
}

std::string CGI::Response::getLocation() const
{
	return (_location);
}

int CGI::Response::getStatus() const
{
	return (_status);
}
