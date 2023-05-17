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
// Method
const int         METHOD_COUNT = 4;
const std::string METHOD_STR[METHOD_COUNT] = {"NONE", "GET", "POST", "DELETE"};

// Status Code
const std::map<int, std::string> STATUS_CODE;

} // namespace HTTP

#endif
