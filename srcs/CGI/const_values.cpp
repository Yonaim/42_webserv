#include "CGI/const_values.hpp"

// special token
const std::string CGI::NL = "\n";
const std::string HTTP::LWS = " \t";
const size_t CGI::NL_LEN = CGI::NL.length();

// meta-variables
const std::string _META_VARIABLES[]
	= {"AUTH_TYPE",      "CONTENT_LENGTH",  "CONTENT_TYPE", "GATEWAY_INTERFACE",
	   "PATH_INFO",      "PATH_TRANSLATED", "QUERY_STRING", "REMOTE_ADDR",
	   "REMOTE_HOST",    "REMOTE_IDENT",    "REMOTE_USER",  "REQUEST_METHOD",
	   "SCRIPT_NAME",    "SERVER_NAME",     "SERVER_PORT",  "SERVER_PROTOCOL",
	   "SERVER_SOFTWARE"};

const std::vector<std::string> CGI::META_VARIABLES(
	_META_VARIABLES,
	_META_VARIABLES + sizeof(_META_VARIABLES) / sizeof(_META_VARIABLES[0]));
