#include "HTTPResponse.hpp"
#include "../const_values.hpp"
#include <iostream>

const std::string HTTP::Response::_http_version = "HTTP/1.1";

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
	setDefaultValue();
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
