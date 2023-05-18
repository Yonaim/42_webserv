#ifndef HTTPCONSTVALUES_HPP
#define HTTPCONSTVALUES_HPP

#include <map>
#include <string>

enum e_http_method
{
	GET = 0,
	POST,
	DELETE,
};

namespace HTTP
{
// special token
extern const std::string CRLF;
extern const std::string SP;
extern const int CRLF_LEN;
extern const int SP_LEN;

// Method
extern const int METHOD_COUNT;
extern const std::string METHOD_STR[];

// Status Code
extern const std::map<int, std::string> STATUS_CODE;

} // namespace HTTP

#endif
