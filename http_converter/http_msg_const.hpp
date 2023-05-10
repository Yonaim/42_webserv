#ifndef HTTP_MSG_CONST
#define HTTP_MSG_CONST

#include <map>
#include <string>
#include <vector>

namespace http_msg
{
typedef std::string                                str_t;
typedef std::vector<std::string>                   str_vec_t;
typedef std::map<str_t, str_vec_t> str_map_t;
typedef std::map<str_t, str_vec_t>::const_iterator map_iter_t;

const std::string CRLF = "\r\n";
const std::string SP = " ";
const int         CRLF_LEN = 2;
const int         SP_LEN = 1;

namespace method
{
const int         kCount = 3;
const std::string kTypeStr[kCount] = {"GET", "POST", "DELET"};

enum e_type
{
	kGet,
	kPost,
	kDelete
};
} // namespace method

enum e_errcode
{
	ERR_START_LINE_PARSING,
	ERR_HEADER_PARSING,
	ERR_BODY_PARSING
};

const std::string kErrMsg[3] = {"start line parsing failed", "header parsing failed",
						  "body parsing failed"};
} // namespace http_msg

#endif