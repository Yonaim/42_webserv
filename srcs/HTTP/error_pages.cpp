#include "./const_values.hpp"
#include "utils/string.hpp"
#include <sstream>

using namespace HTTP;

std::string generateErrorPage(const int code)
{
	if (code < 300 || code >= 600)
		throw(std::runtime_error(std::string(__func__)
								 + ": invalid HTTP response code "
								 + toStr(code)));

	std::stringstream code_message;
	code_message << code << " " << STATUS_CODE.find(code)->second;
	std::stringstream body;
	body << "<html>" << CRLF;
	body << "<head><title>" << code_message.str() << "</title></head>" << CRLF;
	body << "<body>" << CRLF;
	body << "<center><h1>" << code_message.str() << "</h1></center>" << CRLF;
	return (body.str());
}
