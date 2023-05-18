#include "HTTPResponse.hpp"
#include "../const_values.hpp"
#include <iostream>

const std::string HTTP::Response::_http_version = "1.1";

HTTP::Response::Response(void)
{
	initGeneralHeaderFields();
	initResponseHeaderFields();
	initEntityHeaderFields();
}

HTTP::Response::Response(HTTP::Response const &other)
	: _response(other._response), _status_code(other._status_code),
	  _reason_phrase(other._reason_phrase), _header(other._header),
	  _body(other._body)
{
}

HTTP::Response &HTTP::Response::operator=(HTTP::Response const &other)
{
	if (this != &other)
	{
		_response = other._response;
		_status_code = other._status_code;
		_reason_phrase = other._reason_phrase;
		_header = other._header;
		_body = other._body;
	}
	return (*this);
}

HTTP::Response::~Response(void)
{
}

// convert to string
std::string HTTP::Response::toString(void)
{
	if (isComplete() == false)
		throw(std::logic_error("needs more data to make response message"));
	setDefault();
	_response.clear();
	makeStatusLine();
	makeHeader();
	makeBody();
	return (_response);
}

bool HTTP::Response::isComplete(void) const
{
	// TODO: Implementation
	return (true);
}

void HTTP::Response::makeStatusLine(void)
{
	_response.append(_http_version);
	_response.append(SP);
	_response.append(_status_code);
	_response.append(SP);
	_response.append(_reason_phrase);
}

void HTTP::Response::makeHeader(void)
{
	for (Header::const_iterator iter = _header.begin(); iter != _header.end();
		 ++iter)
	{
		if (iter->second.empty())
			continue;
		_response.append(iter->first);
		_response.append(": ");
		// TODO: 헤더의 값 여러개를 문자열에 추가
		_response.append(CRLF);
	}
	_response.append(CRLF);
}

void HTTP::Response::makeBody(void)
{
	_response.append(_body);
}

void HTTP::Response::setDate(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::setDefault(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::setStatus(int status_code)
{
	(void)status_code;
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::setConnection(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::setContentLength(void)
{
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::setValue(std::string const &key, std::string const &val)
{
	(void)key;
	(void)val;
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
}

void HTTP::Response::initGeneralHeaderFields(void)
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

void HTTP::Response::initResponseHeaderFields(void)
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

void HTTP::Response::initEntityHeaderFields(void)
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
