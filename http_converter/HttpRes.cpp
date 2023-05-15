#include "HttpRes.hpp"

using http_msg::HttpRes;
using http_msg::str_t;
using http_msg::str_vec_t;

const str_t HttpRes::_http_version = "HTTP/1.1";

HttpRes::HttpRes()
{
	// general header
	initGeneralHeaderFields();
	initResponseHeaderFields();
	initEntityHeaderFields();
}

// private copy constructor
HttpRes::HttpRes(HttpRes const &other)
{
}

// private assignment oeprator overloading
HttpRes &HttpRes::operator=(HttpRes const &other)
{
}

HttpRes::~HttpRes()
{
}

str_t HttpRes::toString(void)
{
	if (isComplete() == false)
		throw(std::logic_error("needs more data to make response message"));
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
	_response.append(_http_version);
	_response.append(SP);
	_response.append(_status_code);
	_response.append(SP);
	_response.append(_reason_phrase);
}

void HttpRes::makeHeader()
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

void HttpRes::makeBody()
{
	_response.append(_body);
}

// setter
void HttpRes::setStatus(int status_code)
{
	StatusModule &status_manager = StatusModule::GetInstance();

	_status_code = status_manager.toStr(status_code);
	_reason_phrase = status_manager.getReasonPhrase(status_code);
}

void HttpRes::setValue(std::string const &key, std::string const &val)
{
	_header_iterator iter = _header.find(key);

	if (iter == _header.end())
		throw(std::logic_error("wrong header for response."));
	else if (iter->second.length() > 0)
		iter->second.append(", ");
	iter->second.append(val);
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
	_header.insert(std::make_pair("Server", ""));
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
