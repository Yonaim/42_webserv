#include "StatusModule.hpp"

// public
StatusModule &StatusModule::GetInstance()
{
	static StatusModule statusManager;

	return (statusManager);
}

std::string StatusModule::toStr(int status_code) const
{
	std::string res;
	char buff[4];
	int  i = 3;
	int  temp = status_code;

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
	res = buff;
	return (res);
}

std::string const &StatusModule::getReasonPhrase(int status_code) const
{
	_iterator iter = _reason_phrase.find(status_code);

	if (iter == _reason_phrase.end())
		return ("");
	else
		return (iter->second);
}

// private
StatusModule::StatusModule()
{
	initReasonPhrase();
}

StatusModule::~StatusModule()
{
}

StatusModule::StatusModule(StatusModule const &other)
{
	_reason_phrase = other._reason_phrase;
}

StatusModule &StatusModule::operator=(StatusModule const &other)
{
	if (this != &other)
	{
		_reason_phrase = other._reason_phrase;
	}
	return (*this);
}

void StatusModule::initReasonPhrase()
{
	_reason_phrase.insert(std::make_pair(100, "Continue"));
	_reason_phrase.insert(std::make_pair(101, "Switching Protocols"));
	_reason_phrase.insert(std::make_pair(200, "Ok"));
	_reason_phrase.insert(std::make_pair(201, "Created"));
	_reason_phrase.insert(std::make_pair(202, "Accepted"));
	_reason_phrase.insert(std::make_pair(203, "Non-Authoritative Information"));
	_reason_phrase.insert(std::make_pair(204, "No Content"));
	_reason_phrase.insert(std::make_pair(205, "Reset Content"));
	_reason_phrase.insert(std::make_pair(206, "Partial Content"));
	_reason_phrase.insert(std::make_pair(300, "Multiple Choices"));
	_reason_phrase.insert(std::make_pair(301, "Moved Permanently"));
	_reason_phrase.insert(std::make_pair(302, "Found"));
	_reason_phrase.insert(std::make_pair(303, "See Other"));
	_reason_phrase.insert(std::make_pair(304, "Not Modified"));
	_reason_phrase.insert(std::make_pair(305, "Use Proxy"));
	_reason_phrase.insert(std::make_pair(307, "Temporary Redirect"));
	_reason_phrase.insert(std::make_pair(400, "Bad Request"));
	_reason_phrase.insert(std::make_pair(401, "Unauthorized"));
	_reason_phrase.insert(std::make_pair(402, "Payment Required"));
	_reason_phrase.insert(std::make_pair(403, "Forbidden"));
	_reason_phrase.insert(std::make_pair(404, "Not Found"));
	_reason_phrase.insert(std::make_pair(405, "Method Not Allowed"));
	_reason_phrase.insert(std::make_pair(406, "Not Acceptable"));
	_reason_phrase.insert(std::make_pair(407, "Proxy Authentication Required"));
	_reason_phrase.insert(std::make_pair(408, "Request Time-out"));
	_reason_phrase.insert(std::make_pair(409, "Conflict"));
	_reason_phrase.insert(std::make_pair(410, "Gone"));
	_reason_phrase.insert(std::make_pair(411, "Length Required"));
	_reason_phrase.insert(std::make_pair(412, "Precondition Failed"));
	_reason_phrase.insert(std::make_pair(413, "Request Entity Too Large"));
	_reason_phrase.insert(std::make_pair(414, "Request-URI Too Large"));
	_reason_phrase.insert(std::make_pair(415, "Unsupported Media Type"));
	_reason_phrase.insert(
		std::make_pair(416, "Requested range not satisfiable"));
	_reason_phrase.insert(std::make_pair(417, "Expectation Failed"));
	_reason_phrase.insert(std::make_pair(500, "Internal Server Error"));
	_reason_phrase.insert(std::make_pair(501, "Not Implemented"));
	_reason_phrase.insert(std::make_pair(502, "Bad Gateway"));
	_reason_phrase.insert(std::make_pair(503, "Service Unavailable"));
	_reason_phrase.insert(std::make_pair(504, "Gateway Time-out"));
	_reason_phrase.insert(std::make_pair(505, "HTTP Version not supported"));
}
