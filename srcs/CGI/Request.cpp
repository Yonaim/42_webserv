#include "CGI/Request.hpp"

using namespace CGI;

static const char *meta_variable_names[]
	= {"AUTH_TYPE",      "CONTENT_LENGTH",  "CONTENT_TYPE", "GATEWAY_INTERFACE",
	   "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING", "REMOTE_ADDR",
	   "REMOTE_HOST",    "REMOTE_IDENT",    "REMOTE_USER",  "REQUEST_METHOD",
	   "SCRIPT_NAME",    "SERVER_NAME",     "SERVER_PORT",  "SERVER_PROTOCOL",
	   "SERVER_SOFTWARE"};

Request::Request()
{
	for (size_t i = 0; i < _n_meta_variables; i++)
		_meta_variables.insert(
			std::pair<std::string, std::string>(meta_variable_names[i], ""));
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

const char **Request::getEnv(void) const
{
	// TODO: getEnv 구현
	return (NULL);
}