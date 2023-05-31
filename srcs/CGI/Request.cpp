#include "CGI/Request.hpp"
#include "CGI/const_values.hpp"
#include "utils/string.hpp"
using namespace CGI;

const std::string Request::_version = "1.1";

Request::Request(const HTTP::Request &http_req,
				 const std::string &resource_path)
	: _logger(async::Logger::getLogger("CGIRequest"))
{
	for (size_t i = 0; i < META_VARIABLES.size(); i++)
	{
		const std::string &name = META_VARIABLES[i];
		_meta_variables.insert(std::pair<std::string, std::string>(name, ""));
	}
	std::string content_length = toStr<size_t>(http_req.getBody().size());
	std::string content_type = http_req.hasHeaderValue("Content-Type")
								   ? http_req.getHeaderValue("Content-Type", 0)
								   : "";
	std::string host_header = http_req.getHeaderValue("Host", 0);
	size_t colon_pos = host_header.find(':');
	std::string server_name;
	std::string server_port;
	if (colon_pos == std::string::npos)
	{
		server_name = host_header;
		server_port = "80";
	}
	else
	{
		server_name = getfrontstr(host_header, colon_pos);
		server_port = getbackstr(host_header, colon_pos + 1);
	}

	_meta_variables["CONTENT_LENGTH"] = content_length;
	_meta_variables["CONTENT_TYPE"] = content_type;
	_meta_variables["GATEWAY_INTERFACE"] = "CGI/" + _version;
	_meta_variables["PATH_INFO"] = http_req.getURIPath();
	_meta_variables["PATH_TRANSLATED"] = resource_path;
	_meta_variables["QUERY_STRING"] = http_req.getQueryString();
	_meta_variables["REQUEST_METHOD"] = http_req.getMethodString();
	_meta_variables["SERVER_PROTOCOL"] = "HTTP/1.1";
	_meta_variables["SERVER_NAME"] = server_name;
	_meta_variables["SERVER_PORT"] = server_port;
}

Request::~Request()
{
}

Request::Request(const Request &orig)
	: _message_body(orig._message_body), _logger(orig._logger)
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

const std::string &Request::getPath() const
{
	return (_meta_variables.find("PATH_TRANSLATED")->second);
}

void Request::setMetaVariable(const std::string &name, const std::string &value)
{
	_meta_variables[name] = value;
}

void Request::setMessageBody(std::string message_body)
{
	_message_body = message_body;
}
