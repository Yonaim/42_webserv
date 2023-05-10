#include "HttpRes.hpp"

using http_msg::HttpRes;
using http_msg::str_t;
using http_msg::str_vec_t;

HttpRes::HttpRes(int status) : _version("HTTP/1.1")
{
}

str_t HttpRes::toString(void)
{
}
