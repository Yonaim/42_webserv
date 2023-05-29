#ifndef HTTP_ERRORPAGES_HPP
#define HTTP_ERRORPAGES_HPP

#include <string>

namespace HTTP
{

std::string generateErrorPage(const int code);

} // namespace HTTP

#endif
