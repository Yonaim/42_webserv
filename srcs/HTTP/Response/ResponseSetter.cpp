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
	tm *tm_gmt = gmtime(&cur_time);

	static const std::string month[]
		= {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static const std::string day_name[]
		= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	std::string date(day_name[tm_gmt->tm_wday] + ", ");

	date.append(toStr(tm_gmt->tm_mday / 10, tm_gmt->tm_mday % 10) + " ");
	date.append(month[tm_gmt->tm_mon] + " ");
	date.append(toStr(tm_gmt->tm_year + 1900) + " ");
	date.append(toStr(tm_gmt->tm_hour / 10, tm_gmt->tm_hour % 10) + ":");
	date.append(toStr(tm_gmt->tm_min / 10, tm_gmt->tm_min % 10) + ":");
	date.append(toStr(tm_gmt->tm_sec / 10, tm_gmt->tm_sec % 10) + " GMT");
	setValue("Date", date);
}

void Response::setStatus(int status_code)
{
	char buff[4];
	int i = 3;
	int temp = status_code;

	if (status_code < 0)
		throw(std::runtime_error("wrong status code"));
	buff[i--] = '\0';
	while (temp > 0 && i >= 0)
	{
		buff[i--] = temp % 10 + '0';
		temp /= 10;
	}
	if (temp != 0 && i != -1)
		throw(std::runtime_error("wrong status code"));
	_status_code = buff;

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
	if (length > 0)
		setValue("Content-Length", uintToStr(length));
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
