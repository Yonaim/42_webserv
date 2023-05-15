#include "HttpRes.hpp"

using http_msg::HttpRes;

HttpRes::HttpRes() : _request(NULL)
{
}

HttpRes::HttpRes(HttpReq const *request) : _request(request)
{
	// general header
	initGeneralHeaderFields();
	initResponseHeaderFields();
	initEntityHeaderFields();
}

HttpRes::HttpRes(HttpRes const &other) : _request(other._request)
{
	_response = other._response;
	_status_code = other._status_code;
	_reason_phrase = other._reason_phrase;
	_header = other._header;
	_body = other._body;
}

HttpRes &HttpRes::operator=(HttpRes const &other)
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

HttpRes::~HttpRes()
{
}

// convert to string
std::string HttpRes::toString(void)
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

bool HttpRes::isComplete() const
{
}

void HttpRes::makeStatusLine()
{
}

void HttpRes::makeHeader()
{
}

void HttpRes::makeBody()
{
	_response.append(_body);
}

void HttpRes::initGeneralHeaderFields()
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

void HttpRes::initResponseHeaderFields()
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

void HttpRes::initEntityHeaderFields()
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
