#ifndef HTTP_MSG_CONST
#define HTTP_MSG_CONST

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <exception>

namespace http_msg
{
// typedef
typedef std::string                str_t;
typedef std::vector<std::string>   str_vec_t;
typedef std::map<str_t, str_vec_t> str_map_t;

// special word
const std::string CRLF = "\r\n";
const std::string SP = " ";
const int         CRLF_LEN = 2;
const int         SP_LEN = 1;

// method
enum eMethodType
{
	kGet,
	kPost,
	kDelete
};
const int         kMethodCount = 3;
const std::string kMethodStr[kCount] = {"GET", "POST", "DELETE"};

// header fields
const std::string kGeneralHeaderFields[]
	= {"cache-control",     "connection", "date", "pragma", "trailer",
	   "transfer-encoding", "upgrade",    "via",  "warning"};
const std::string kRequestHeaderFields[] = {"accept",
											"accept-charset",
											"accept-encoding",
											"accept-language",
											"authorization",
											"expect",
											"from",
											"host",
											"if-match",
											"if-modified-since",
											"if-none-match",
											"if-range",
											"if-unmodified-since",
											"max-forwards",
											"proxy-authorization",
											"range",
											"referer",
											"te",
											"user-agent"};
const std::string kResponseHeaderFields[] = {
	"accept-ranges", "age",    "etag", "location",        "proxy-authenticate",
	"retry-after",   "server", "vary", "www-authenticate"};
const std::string kEntityHeaderFields[]
	= {"allow",          "content-encoding", "content-language",
	   "content-length", "content-location", "content-md5",
	   "content-range",  "content-type",     "expires",
	   "last-modified",  "extension-header"};

// error message
const std::string kErrStartLineParsing = "failed to parse the request line.";
const std::string kErrHeaderParsing = "failed to parse the header fields.";
const std::string kErrBodyParsing = "failed to parse the entity body.";
} // namespace http_msg

#endif