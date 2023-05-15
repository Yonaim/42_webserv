#include "HttpRes.hpp"

using http_msg::HttpRes;
const std::string HttpRes::_http_version = "HTTP/1.1";

void HttpRes::setDefault()
{
	setConnection();
	setContentLength();
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

void HttpRes::setStatus(int status_code)
{
	StatusModule &status_manager = StatusModule::GetInstance();

	_status_code = status_manager.toStr(status_code);
	_reason_phrase = status_manager.getReasonPhrase(status_code);
}

void HttpRes::setConnection()
{
	if (_request->hasHeaderVal("Connection", "close") == true)
		setValue("Connection", "close");
	else
		setValue("Connection", "keep-alive");
}

void HttpRes::setContentLength()
{
	if (_body.length() > 0)
		setValue("Content-Length", uintToStr(_body.length()));
}

