#ifndef CONSTVALUES_HPP
#define CONSTVALUES_HPP

#include "BidiMap.hpp"
#include <map>
#include <string>

enum e_http_method
{
	METHOD_NONE = 0,
	METHOD_GET,
	METHOD_HEAD,
	METHOD_POST,
	METHOD_PUT,
	METHOD_DELETE,
};

namespace HTTP
{
// special token
extern const std::string CRLF;
extern const std::string SP;
extern const std::string LWS;
extern const size_t CRLF_LEN;
extern const size_t SP_LEN;
extern const size_t LWS_LEN;

// method
extern const BidiMap<std::string, int> METHOD;

// status Code
extern const std::map<int, std::string> STATUS_CODE;

// mime type
extern const std::map<std::string, std::string> MIME_TYPE;

} // namespace HTTP

#endif
