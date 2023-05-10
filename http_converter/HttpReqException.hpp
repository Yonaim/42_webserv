#ifndef HTTP_REQ_EXCEPTION_HPP
#define HTTP_REQ_EXCEPTION_HPP
#include "http_msg_const.hpp"

namespace http_msg
{
class HttpReqException : public std::exception
{
  public:
	HttpReqException(int errcode);
	HttpReqException(int errcode, str_vec_t *content);
    void    deleteAll();

  private:
	str_vec_t *_content;
	int        _errcode;
};
} // namespace http_msg

#endif