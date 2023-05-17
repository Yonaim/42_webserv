#include "HTTPResponse.hpp"

HTTP::Response::Response() : _request(NULL)
{
}

HTTP::Response::Response(HttpReq const *request) : _request(request)
{
	// general header
	initGeneralHeaderFields();
	initResponseHeaderFields();
	initEntityHeaderFields();
}

HTTP::Response::Response(Response const &other) : _request(other._request)
{
	_response = other._response;
	_status_code = other._status_code;
	_reason_phrase = other._reason_phrase;
	_header = other._header;
	_body = other._body;
}

Response &HTTP::Response::operator=(Response const &other)
{
	if (this != &other)
	{
		*(const_cast<HttpReq **>(&_request))
			= const_cast<HttpReq *>(other._request);
		_response = other._response;
		_status_code = other._status_code;
		_reason_phrase = other._reason_phrase;
		_header = other._header;
		_body = other._body;
	}
	return (*this);
}

HTTP::Response::~Response()
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

bool HTTP::Response::isComplete() const
{
}

void HTTP::Response::makeStatusLine()
{
	_response.append(_http_version);
	_response.append(SP);
	_response.append(_status_code);
	_response.append(SP);
	_response.append(_reason_phrase);
}

void HTTP::Response::makeHeader()
{
	_header_iterator iter = _header.begin();

	for (iter; iter != _header.end(); ++iter)
	{
		if (iter->second.length() == 0)
			continue;
		_response.append(iter->first);
		_response.append(": ");
		_response.append(iter->second);
		_response.append(CRLF);
	}
	_response.append(CRLF);
}

void HTTP::Response::makeBody()
{
	_response.append(_body);
}

void HTTP::Response::initGeneralHeaderFields()
{
	_header.insert(std::make_pair("Cache-Control", ""));
	_header.insert(std::make_pair("Connection", ""));
	_header.insert(std::make_pair("Date", ""));
	_header.insert(std::make_pair("Pragma", ""));
	_header.insert(std::make_pair("Trailer", ""));
	_header.insert(std::make_pair("Transfer-Encoding", ""));
	_header.insert(std::make_pair("Upgrade", ""));
	_header.insert(std::make_pair("Via", ""));
	_header.insert(std::make_pair("Warning", ""));
}

void HTTP::Response::initResponseHeaderFields()
{
	_header.insert(std::make_pair("Accept-Ranges", ""));
	_header.insert(std::make_pair("Age", ""));
	_header.insert(std::make_pair("Etag", ""));
	_header.insert(std::make_pair("Location", ""));
	_header.insert(std::make_pair("Proxy-Authenticate", ""));
	_header.insert(std::make_pair("Retry-After", ""));
	_header.insert(std::make_pair("Server", "webserv/1.0.0"));
	_header.insert(std::make_pair("Vary", ""));
	_header.insert(std::make_pair("WWW-Authenticate", ""));
}

void HTTP::Response::initEntityHeaderFields()
{
	_header.insert(std::make_pair("Allow", ""));
	_header.insert(std::make_pair("Content-Encoding", ""));
	_header.insert(std::make_pair("Content-Length", ""));
	_header.insert(std::make_pair("Content-Location", ""));
	_header.insert(std::make_pair("Content-Range", ""));
	_header.insert(std::make_pair("Content-Type", ""));
	_header.insert(std::make_pair("Expires", ""));
	_header.insert(std::make_pair("Last-Modified", ""));
}
