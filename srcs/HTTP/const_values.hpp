#ifndef CONSTVALUES_HPP
#define CONSTVALUES_HPP

#include <map>
#include <string>

enum e_http_method
{
	METHOD_NONE = 0,
	METHOD_GET,
	METHOD_HEAD,
	METHOD_POST,
	METHOD_DELETE,
};

namespace HTTP
{
// special token
extern const std::string CRLF;
extern const std::string SP;
extern const int CRLF_LEN;
extern const int SP_LEN;

// method
extern const int METHOD_COUNT;
extern const std::string METHOD_STR[];
// TODO: converter 모듈에서 METHOD_STR 대신 METHOD를 사용하도록 수정
extern const std::map<std::string, int> METHOD;

// status Code
extern const std::map<int, std::string> STATUS_CODE;

// mime type
extern const std::map<std::string, std::string> MIME_TYPE;

} // namespace HTTP

#endif
