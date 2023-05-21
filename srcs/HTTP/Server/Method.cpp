#include "../const_values.hpp"
#include "../utils.hpp"
#include "HTTP/Server.hpp"
#include "HTTP/ServerException.hpp"
#include "async/FileIOProcessor.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

void HTTP::Server::setErrorPage(HTTP::Response &response, int status_code)
{
	const std::map<int, std::string>::const_iterator iter
		= _error_pages.find(status_code);
	std::string error_page;

	if (iter == _error_pages.end())
		error_page = _error_pages.at(0);
	else
		error_page = iter->second;
	// TODO:
	// 파일을 읽고, 내용을 string에 저장하면
	std::string content;
	response.setBody(content);
	response.setStatus(status_code);
}
