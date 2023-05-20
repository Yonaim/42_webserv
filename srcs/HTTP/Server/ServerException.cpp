#include "HTTP/ServerException.hpp"
#include "const_values.hpp"

using namespace HTTP;

static const char *what(int status_code)
{
	std::map<int, std::string>::const_iterator iter
		= STATUS_CODE.find(status_code);

	if (iter == STATUS_CODE.end())
		return ("Unknown Server exception");
	return (iter->second.c_str());
}

Server::ServerException::ServerException(int status_code)
	: std::runtime_error(::what(status_code)), _status_code(status_code)
{
}

int Server::ServerException::getStatusCode() const
{
	return (_status_code);
}
