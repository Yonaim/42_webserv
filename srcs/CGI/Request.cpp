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

char *const *Request::getEnv(void) const
{
	char **env = new char *[_n_meta_variables + 1];
	for (size_t i = 0; i < _n_meta_variables; i++)
	{
		const std::string &name = meta_variable_names[i];
		const std::string &value = _meta_variables.find(name)->second;
		env[i] = strdup((name + '=' + value).c_str());
	}
	env[_n_meta_variables] = NULL;
	return ((char *const *)env);
}

const std::string &Request::getMessageBody(void) const
{
	return (_message_body);
}

const std::string Request::getPath() const
{
	// TODO: getPath() 구현
	std::string str;
	return (str);
}
void Request::setMetaVariable(std::string name, std::string value)
{
	_meta_variables[name] = value;
}
