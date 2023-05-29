#include "CGI/Response.hpp"

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

void CGI::Response::makeResponse(const std::string &cgi_output)
{
	(void)cgi_output;
	// TODO: string으로 받은 Response 메시지 기반으로 CGI/Response 초기화
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