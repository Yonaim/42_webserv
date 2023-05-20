#include "HTTPServerException.hpp"

HTTP::ServerException::ServerException(int status_code)
	: _status_code(status_code)
{
}

int HTTP::ServerException::getStatusCode() const
{
	return (_status_code);
}

const char *HTTP::ServerException::what() const
{
	std::map<int, std::string>::const_iterator iter
		= STATUS_CODE.find(_status_code);

	if (iter == STATUS_CODE.end())
		return ("Unknown Server exception");
	return (iter->second.c_str());
}
