#include "const_values.hpp"
#include <map>

const std::string HTTP::CRLF = "\r\n";
const std::string HTTP::SP = " ";
const int HTTP::CRLF_LEN = 2;
const int HTTP::SP_LEN = 1;

// Method
const int HTTP::METHOD_COUNT = 4;
const std::string HTTP::METHOD_STR[METHOD_COUNT]
	= {"NONE", "GET", "POST", "DELETE"};

static const std::pair<int, std::string> _STATUS_CODE[] = {
	std::pair<int, std::string>(100, "Continue"),
	std::pair<int, std::string>(101, "Switching Protocols"),
	std::pair<int, std::string>(200, "Ok"),
	std::pair<int, std::string>(201, "Created"),
	std::pair<int, std::string>(202, "Accepted"),
	std::pair<int, std::string>(203, "Non-Authoritative Information"),
	std::pair<int, std::string>(204, "No Content"),
	std::pair<int, std::string>(205, "Reset Content"),
	std::pair<int, std::string>(206, "Partial Content"),
	std::pair<int, std::string>(300, "Multiple Choices"),
	std::pair<int, std::string>(301, "Moved Permanently"),
	std::pair<int, std::string>(302, "Found"),
	std::pair<int, std::string>(303, "See Other"),
	std::pair<int, std::string>(304, "Not Modified"),
	std::pair<int, std::string>(305, "Use Proxy"),
	std::pair<int, std::string>(307, "Temporary Redirect"),
	std::pair<int, std::string>(400, "Bad Request"),
	std::pair<int, std::string>(401, "Unauthorized"),
	std::pair<int, std::string>(402, "Payment Required"),
	std::pair<int, std::string>(403, "Forbidden"),
	std::pair<int, std::string>(404, "Not Found"),
	std::pair<int, std::string>(405, "Method Not Allowed"),
	std::pair<int, std::string>(406, "Not Acceptable"),
	std::pair<int, std::string>(407, "Proxy Authentication Required"),
	std::pair<int, std::string>(408, "Request Time-out"),
	std::pair<int, std::string>(409, "Conflict"),
	std::pair<int, std::string>(410, "Gone"),
	std::pair<int, std::string>(411, "Length Required"),
	std::pair<int, std::string>(412, "Precondition Failed"),
	std::pair<int, std::string>(413, "Request Entity Too Large"),
	std::pair<int, std::string>(414, "Request-URI Too Large"),
	std::pair<int, std::string>(415, "Unsupported Media Type"),
	std::pair<int, std::string>(416, "Requested range not satisfiable"),
	std::pair<int, std::string>(417, "Expectation Failed"),
	std::pair<int, std::string>(500, "Internal Server Error"),
	std::pair<int, std::string>(501, "Not Implemented"),
	std::pair<int, std::string>(502, "Bad Gateway"),
	std::pair<int, std::string>(503, "Service Unavailable"),
	std::pair<int, std::string>(504, "Gateway Time-out"),
	std::pair<int, std::string>(505, "HTTP Version not supported"),
};

const std::map<int, std::string> HTTP::STATUS_CODE(
	_STATUS_CODE,
	_STATUS_CODE + sizeof(_STATUS_CODE) / sizeof(_STATUS_CODE[0]));
