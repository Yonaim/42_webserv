#include "HttpRes.hpp"

using http_msg::HttpRes;
using http_msg::str_t;
using http_msg::str_vec_t;

HttpRes::HttpRes()
{
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

// setter
void HttpRes::setStatus(int status_code)
{
	StatusModule &status_manager = StatusModule::GetInstance();

	_status_code = status_manager.toStr(status_code);
	_reason_phrase = status_manager.getReasonPhrase(status_code);
}

str_t HttpRes::toString(void)
{
	try
	{
		makeStatusLine();
		makeHeader();
		makeBody();
	}
	catch (std::exception &e)
	{
	}
}

void HttpRes::makeStatusLine()
{
}

void HttpRes::makeHeader()
{
}

void HttpRes::makeBody()
{
}
