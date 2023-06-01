#include "CGI/Request.hpp"
#include "CGI/const_values.hpp"
#include "utils/string.hpp"
#include <algorithm>

using namespace CGI;

const std::string Request::_version = "1.1";

Request::Request(const HTTP::Request &http_req,
				 const std::string &resource_path)
	: _message_body(http_req.getBody()),
	  _logger(async::Logger::getLogger("CGIRequest"))
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

	const Header &header = http_req.getHeader();
	for (Header::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		if (!isProtocolSpecificHeader(it->first))
			continue;

		const std::string http_meta_variable_name = toHTTPvarname(it->first);
		std::string value;
		for (std::vector<std::string>::const_iterator vec_itr
			 = it->second.begin();
			 vec_itr != it->second.end(); ++vec_itr)
		{
			value += *vec_itr;
			if (vec_itr + 1 != it->second.end())
				value += ", ";
		}
		addMetaVariable(http_meta_variable_name, value);
		_logger << async::verbose << "name:" << http_meta_variable_name << ": "
				<< value;
	}

	for (std::map<std::string, std::string>::iterator it
		 = _meta_variables.begin();
		 it != _meta_variables.end(); it++)
		_logger << async::verbose << "CGI metavariable \"" << it->first
				<< "\"=\"" << it->second << "\"";
}

Request::~Request()
{
}

Request::Request(const Request &orig)
	: _meta_variables(orig._meta_variables), _message_body(orig._message_body),
	  _logger(orig._logger)
{
}

bool Request::isProtocolSpecificHeader(const std::string &name) const
{
	for (std::vector<std::string>::const_iterator it
		 = NON_PROTOCOL_SPECIFIC_HEADERS.begin();
		 it != NON_PROTOCOL_SPECIFIC_HEADERS.end(); ++it)
	{
		if (name == *it)
			return (false);
	}
	return (true);
}

std::string Request::toHTTPvarname(const std::string &var_name) const
{
	static const std::string prefix = "HTTP_";
	std::string http_var_name = var_name;

	std::transform(http_var_name.begin(), http_var_name.end(),
				   http_var_name.begin(), ::toupper);
	std::replace(http_var_name.begin(), http_var_name.end(), '-', '_');
	return (prefix + http_var_name);
}

const Request &Request::operator=(const Request &orig)
{
	if (this != &orig)
	{
		_meta_variables = orig._meta_variables;
		_message_body = orig._message_body;
	}
	return (*this);
}

char *const *Request::getEnv(void) const
{
	char **env = new char *[_meta_variables.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it
		 = _meta_variables.begin();
		 it != _meta_variables.end(); ++it)
	{
		const std::string &name = it->first;
		const std::string &value = it->second;
		env[i++] = duplicateStr((name + "=" + value));
	}
	env[_meta_variables.size()] = NULL;
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

void Request::addMetaVariable(const std::string &name, const std::string &value)
{
	if (_meta_variables.find(name) == _meta_variables.end())
		_meta_variables.insert(std::pair<std::string, std::string>(name, ""));
	_meta_variables[name] = value;
}
