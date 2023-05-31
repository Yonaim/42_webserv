#ifndef CGI_CONSTVALUES_HPP
#define CGI_CONSTVALUES_HPP

#include <string>
#include <vector>

namespace CGI
{
// special token
extern const std::string CRLF;
extern const size_t CRLF_LEN;
extern const std::string LWS;

// meta-variables
extern const std::vector<std::string> META_VARIABLES;

extern const std::vector<std::string> NON_PROTOCOL_SPECIFIC_HEADERS;

} // namespace CGI

#endif
