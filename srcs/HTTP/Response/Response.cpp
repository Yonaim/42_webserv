#include "HTTP/Response.hpp"
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
	_response.clear();
	makeStatusLine();
	makeHeader();
	makeBody();
	return (_response);
}

void HTTP::Response::makeStatusLine(void)
{
	_response.append(_http_version);
	_response.append(SP);
	_response.append(_status_code);
	_response.append(SP);
	_response.append(_reason_phrase);
	_response.append(CRLF);
}

void HTTP::Response::makeHeader(void)
{
	for (Header::const_iterator it = _header.begin(); it != _header.end(); ++it)
	{
		if (it->second.empty())
			continue;

		std::string to_append(it->first + ": ");

		const std::vector<std::string> values = _header.getValues(it->first);
		for (std::vector<std::string>::const_iterator val_it = values.begin();
			 val_it != values.end(); ++val_it)
		{
			to_append += *val_it;
			if (val_it + 1 != values.end())
				to_append += ", ";
		}
		to_append += CRLF;
		_response.append(to_append);
	}
	_response.append(CRLF);
}

void HTTP::Response::makeBody(void)
{
	_response.append(_body);
}
