#include "../const_values.hpp"
#include "HTTP/Response.hpp"
#include "utils/string.hpp"
#include <ctime>
#include <iostream>

using namespace HTTP;

void Response::setValue(const std::string &name, const std::string &value)
{
	_header.insert(name, value);
}

void Response::setDate(void)
{
	time_t cur_time = time(NULL);
	tm *gmt_time = gmtime(&cur_time);
	static char date[100];

	strftime(date, sizeof(date), "%a, %d %b %Y %T GMT", gmt_time);
	setValue("Date", date);
}

void Response::setStatus(int status_code)
{
	_status_code = toStr<int>(status_code);
	std::map<int, std::string>::const_iterator it
		= STATUS_CODE.find(status_code);
	if (it == STATUS_CODE.end())
		throw(std::runtime_error("wrong status code"));
	_reason_phrase = it->second;
}

void Response::setContent(const std::string &content,
						  const std::string &file_path)
{
	setContentType(file_path);
	setContentLength(content.length());
	setBody(content);
}

void Response::setContentType(const std::string &file_path)
{
	size_t separator_idx = file_path.find_last_of('.');
	if (separator_idx != std::string::npos)
	{
		const std::string extension = file_path.substr(separator_idx + 1);
		const std::map<std::string, std::string>::const_iterator it
			= MIME_TYPE.find(extension);
		if (it == MIME_TYPE.end())
			// default mime-type
			setValue("Content-Type", "application/octet-stream");
		else
			setValue("Content-Type", it->second);
	}
}

void Response::setContentLength(void)
{
	if (_body.length() > 0)
		setValue("Content-Length", uintToStr(_body.length()));
}

void Response::setContentLength(size_t length)
{
	setValue("Content-Length", toStr<size_t>(length));
}

void Response::setConnection(bool is_persistent)
{
	if (is_persistent)
		setValue("Connection", "keep-alive");
	else
		setValue("Connection", "close");
}

void Response::setBody(const std::string &body)
{
	_body = body;
}

void Response::setLocation(const std::string &uri)
{
	std::vector<std::string> uri_list(1, uri);
	_header.assign("Location", uri_list);
}
