#ifndef HTTP_SERVER_EXCEPTION_HPP
#define HTTP_SERVER_EXCEPTION_HPP

#include "HTTPResponse.hpp"
#include "const_values.hpp"
#include <exception>
#include <string>

namespace HTTP
{
class ServerException : public std::exception
{
  private:
	int _status_code;

  public:
	ServerException(int status_code);
	int getStatusCode() const;
	virtual const char *what() const throw();
};
} // namespace HTTP
// 예외처리를 어떻게 할 것인가.
// 오류가 있을때 어떻게 response를 결정할지...
// status code에 따른 에러 페이지 결정
// 에러 페이지 열고 -> string에 저장
// 해당 string을 response의 body에 저장
// response의 status code response의 status_code에 저장

#endif