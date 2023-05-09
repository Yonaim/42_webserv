#ifndef HTTP_MSG_CONST
#define HTTP_MSG_CONST

#include <map>
#include <vector>
#include <string>

namespace http_msg
{
typedef std::string              str_t;
typedef std::vector<std::string> str_vec_t;

const std::string CRLF = "\r\n";
const std::string SP = " ";

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

enum e_status
{
	k
};

} // namespace http_msg

#endif