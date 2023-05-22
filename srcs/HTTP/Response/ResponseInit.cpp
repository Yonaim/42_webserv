#include "HTTP/Response.hpp"

using namespace HTTP;

void Response::initGeneralHeaderFields(void)
{
	_header.insert("Cache-Control", std::vector<std::string>());
	_header.insert("Connection", std::vector<std::string>());
	_header.insert("Date", std::vector<std::string>());
	_header.insert("Pragma", std::vector<std::string>());
	_header.insert("Trailer", std::vector<std::string>());
	_header.insert("Transfer-Encoding", std::vector<std::string>());
	_header.insert("Upgrade", std::vector<std::string>());
	_header.insert("Via", std::vector<std::string>());
	_header.insert("Warning", std::vector<std::string>());
}

void Response::initResponseHeaderFields(void)
{
	_header.insert("Accept-Ranges", std::vector<std::string>());
	_header.insert("Age", std::vector<std::string>());
	_header.insert("Etag", std::vector<std::string>());
	_header.insert("Location", std::vector<std::string>());
	_header.insert("Proxy-Authenticate", std::vector<std::string>());
	_header.insert("Retry-After", std::vector<std::string>());
	_header.insert("Server", std::vector<std::string>());
	_header.insert("Vary", std::vector<std::string>());
	_header.insert("WWW-Authenticate", std::vector<std::string>());
}

void Response::initEntityHeaderFields(void)
{
	_header.insert("Allow", std::vector<std::string>());
	_header.insert("Content-Encoding", std::vector<std::string>());
	_header.insert("Content-Length", std::vector<std::string>());
	_header.insert("Content-Location", std::vector<std::string>());
	_header.insert("Content-Range", std::vector<std::string>());
	_header.insert("Content-Type", std::vector<std::string>());
	_header.insert("Expires", std::vector<std::string>());
	_header.insert("Last-Modified", std::vector<std::string>());
}
