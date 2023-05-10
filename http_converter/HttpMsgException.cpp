#include "HttpMsgException.hpp"

using http_msg::HttpReqException;

HttpReqException::HttpReqException(int errcode)
	: _errcode(errcode),  _content(NULL)
{
}

HttpReqException::HttpReqException(int errcode, str_vec_t *content)
	: _errcode(errcode), _content(content)
{
}

void HttpReqException::deleteAll()
{
	delete _content;
}

const char *HttpReqException::what() const throw()
{
	return (kErrMsg[_errcode].c_str());
}
