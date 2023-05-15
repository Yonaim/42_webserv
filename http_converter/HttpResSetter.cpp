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

std::string	toStr(int num, int num2)
{
	std::ostringstream	ss;

	ss << num << num2;
	return (ss.str());
}

void HttpRes::setDate()
{
	time_t cur_time = time(NULL);
	tm    *tm_gmt = gmtime(&cur_time);

	static const std::string month[]
		= {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	static const std::string day_name[]
		= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	std::string date(day_name[tm_gmt->tm_wday] + ", ");

	date.append(toStr(tm_gmt->tm_mday / 10, tm_gmt->tm_mday % 10) + " ");
	date.append(month[tm_gmt->tm_mon] + " " + toStr(tm_gmt->tm_year, 1900) + " ");
	date.append(toStr(tm_gmt->tm_hour / 10, tm_gmt->tm_hour % 10) + ":");
	date.append(toStr(tm_gmt->tm_min / 10, tm_gmt->tm_min % 10) + ":");
	date.append(toStr(tm_gmt->tm_sec / 10, tm_gmt->tm_sec % 10) + " GMT");
	setValue("Date", date);
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
