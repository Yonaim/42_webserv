#include "CGI/Request.hpp"
#include "CGI/const_values.hpp"

using namespace CGI;

const std::string Request::_version = "1.1";

Request::Request()
{
	for (size_t i = 0; i < META_VARIABLES.size(); i++)
	{
		std::string name = META_VARIABLES[i];
		_meta_variables.insert(std::pair<std::string, std::string>(name, ""));
	}
	_meta_variables.find("GATEWAY_INTERFACE")->second = "CGI/" + _version;
}

Request::~Request()
{
}

Request::Request(const Request &orig) : _message_body(orig._message_body)
{
	for (size_t i = 0; i < META_VARIABLES.size(); i++)
	{
		const std::string &name = META_VARIABLES[i];
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
		for (size_t i = 0; i < META_VARIABLES.size(); i++)
		{
			const std::string &name = META_VARIABLES[i];
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
	char **env = new char *[META_VARIABLES.size() + 1];
	for (size_t i = 0; i < META_VARIABLES.size(); i++)
	{
		const std::string &name = META_VARIABLES[i];
		const std::string &value = _meta_variables.find(name)->second;
		env[i] = strdup((name + '=' + value).c_str());
	}
	env[META_VARIABLES.size()] = NULL;
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
