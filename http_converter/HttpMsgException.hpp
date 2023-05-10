#ifndef HTTP_MSG_EXCEPTION_HPP
#define HTTP_MSG_EXCEPTION_HPP
#include "http_msg_const.hpp"

namespace http_msg
{
class HttpReqException : public std::exception
{
  public:
	HttpReqException(int errcode);
	HttpReqException(int errcode, str_vec_t *content);

	void        deleteAll();
	const char *what() const throw();

  private:
	int        _errcode;
	str_vec_t *_content;
};
} // namespace http_msg

#endif