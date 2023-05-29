#include "CGI/Request.hpp"

using namespace CGI;

const std::string Request::_version = "1.1";

static const char *meta_variable_names[]
	= {"AUTH_TYPE",      "CONTENT_LENGTH",  "CONTENT_TYPE", "GATEWAY_INTERFACE",
	   "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING", "REMOTE_ADDR",
	   "REMOTE_HOST",    "REMOTE_IDENT",    "REMOTE_USER",  "REQUEST_METHOD",
	   "SCRIPT_NAME",    "SERVER_NAME",     "SERVER_PORT",  "SERVER_PROTOCOL",
	   "SERVER_SOFTWARE"};

Request::Request()
{
	for (size_t i = 0; i < _n_meta_variables; i++)
	{
		std::string name = meta_variable_names[i];
		_meta_variables.insert(std::pair<std::string, std::string>(name, ""));
	}
	_meta_variables.find("GATEWAY_INTERFACE")->second = "CGI/" + _version;
}

Request::~Request()
{
}

Request::Request(const Request &orig) : _message_body(orig._message_body)
{
	for (size_t i = 0; i < _n_meta_variables; i++)
	{
		const std::string &name = meta_variable_names[i];
		const std::string &value = orig._meta_variables.find(name)->second;
		_meta_variables.insert(
			std::pair<std::string, std::string>(name, value));
	}
}

const Request &Request::operator=(const Request &orig)
{
	if (this != &orig)
	{
		_meta_variables.clear();
		for (size_t i = 0; i < _n_meta_variables; i++)
		{
			const std::string &name = meta_variable_names[i];
			const std::string &value = orig._meta_variables.find(name)->second;
			_meta_variables.insert(
				std::pair<std::string, std::string>(name, value));
		}
		_message_body = orig._message_body;
	}
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
	return (_meta_variables.find("PATH_TRANSLATED")->second);
}

void Request::setMetaVariable(std::string name, std::string value)
{
	_meta_variables[name] = value;
}
