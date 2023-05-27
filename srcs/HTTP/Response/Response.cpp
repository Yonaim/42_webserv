#include "HTTP/Response.hpp"
#include "../const_values.hpp"
#include "utils/string.hpp"
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

using namespace HTTP;

const std::string Response::_http_version = "HTTP/1.1";

Response::Response(void) : _logger(async::Logger::getLogger("Response"))
{
	initGeneralHeaderFields();
	initResponseHeaderFields();
	initEntityHeaderFields();
}

Response::Response(Response const &other)
	: _response(other._response), _status_code(other._status_code),
	  _reason_phrase(other._reason_phrase), _header(other._header),
	  _body(other._body), _logger(other._logger)
{
}

Response &Response::operator=(Response const &other)
{
	if (this != &other)
	{
		_response = other._response;
		_status_code = other._status_code;
		_reason_phrase = other._reason_phrase;
		_header = other._header;
		_body = other._body;
	}
	return (*this);
}

Response::~Response(void)
{
}

// convert to string
std::string Response::toString(void)
{
	setDate();
	_response.clear();
	makeStatusLine();
	makeHeader();
	makeBody();
	return (_response);
}

void Response::makeStatusLine(void)
{
	_response.append(_http_version);
	_response.append(SP);
	_response.append(_status_code);
	_response.append(SP);
	_response.append(_reason_phrase);
	_response.append(CRLF);
}

void Response::makeHeader(void)
{
	for (Header::const_iterator it = _header.begin(); it != _header.end(); ++it)
	{
		if (it->second.empty())
			continue;

		std::string to_append(it->first + ": ");

		const std::vector<std::string> values = _header.getValues(it->first);
		for (std::vector<std::string>::const_iterator val_it = values.begin();
			 val_it != values.end(); ++val_it)
		{
			to_append += *val_it;
			if (val_it + 1 != values.end())
				to_append += ", ";
		}
		to_append += CRLF;
		_response.append(to_append);
	}
	_response.append(CRLF);
}

void Response::makeBody(void)
{
	_response.append(_body);
}

void Response::alignAutoIndex(size_t minus_len, int to_align)
{
	if (!(AUTOINDEX_ALIGN_FILE_NAME <= to_align
		  && to_align <= AUTOINDEX_ALIGN_FILE_SIZE))
		throw std::runtime_error("alignAutoIndex: wrong argument(to_align)");

	const int size[] = {51, 20};
	int align_size = size[to_align] - minus_len;
	for (int i = 0; i < align_size; ++i)
		_body.append(" ");
}

void Response::makeDirectoryListing(const std::string &path,
									const std::string &uri)
{
	setContentType("text/html");

	_body.clear();
	_body.append("<html>\n"
				 "<head><title>Index of "
				 + uri
				 + "</title></head>\n"
				   "<body>\n"
				   "<h1>Index of "
				 + uri
				 + "</h1>\n"
				   "<hr><pre><a href=\"../\">../</a>\n");

	/* 각 파일에 대한 내용을 추가 */
	struct stat file_info;
	DIR *dir_stream;
	struct dirent *dir_info;
	std::string file_name;
	std::string file_path;
	std::string file_size;
	char birth_time[50];

	dir_stream = opendir(path.c_str());
	if (dir_stream == NULL) // 실패
		perror("opendir error");

	for (int i = 0; i < 2; ++i)
		dir_info = readdir(dir_stream);
	while ((dir_info = readdir(dir_stream)) != NULL)
	{
		file_name = dir_info->d_name;
		file_path = path + "/" + file_name;
		if (stat(file_path.c_str(), &file_info) == 0)
		{
			if (file_info.st_mode & S_IFDIR)
				file_name += '/';
			_body.append("<a href=\"" + file_name + "\">" + file_name + "</a>");
			strftime(birth_time, sizeof(birth_time), "%d-%b-%Y %R",
					 gmtime(&file_info.st_mtimespec.tv_sec));
			alignAutoIndex(file_name.length(), AUTOINDEX_ALIGN_FILE_NAME);
			_body.append(birth_time);
			if (file_info.st_mode & S_IFDIR)
				file_size = "-";
			else
				file_size = toStr(file_info.st_size);
			alignAutoIndex(file_size.length(), AUTOINDEX_ALIGN_FILE_SIZE);
			_body.append(file_size);
			_body.append("\n");
		}
	}
	closedir(dir_stream);

	_body.append("</pre><hr></body>\n"
				 "</html>\n");

	setContentLength(_body.length());
}
