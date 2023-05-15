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

// error message
const std::string kErrStartLineParsing = "failed to parse the request line.";
const std::string kErrHeaderParsing = "failed to parse the header fields.";
const std::string kErrBodyParsing = "failed to parse the entity body.";
} // namespace http_msg

#endif