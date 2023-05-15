#ifndef HTTP_MSG_CONST
#define HTTP_MSG_CONST

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace http_msg
{
// typedef
typedef std::map<std::string, std::vector<std::string> > str_vec_map_t;

// special token
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
} // namespace http_msg

#endif