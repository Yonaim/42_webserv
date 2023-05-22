#ifndef ERRORPAGES_HPP
#define ERRORPAGES_HPP

#include <string>

namespace HTTP
{

std::string generateErrorPage(const int code);

} // namespace HTTP

#endif
