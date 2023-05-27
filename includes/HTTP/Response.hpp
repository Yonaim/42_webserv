#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "Header.hpp"
#include "async/Logger.hpp"

// 시작줄 : [HTTP 버전] [상태 코드] [사유 구절] # 공백으로 띄워진다.
// 헤더, 빈 줄, 엔티티 본문이 온다.
/*
	HTTP/1.1 200 OK
	Server: nginx/1.23.3
	Date: Wed, 10 May 2023 07:35:44 GMT
	Content-Type: text/html
	Content-Length: 611
	Last-Modified: Wed, 03 May 2023 07:10:36 GMT
	Connection: keep-alive
	ETag: "645208ec-263"
	Accept-Ranges: bytes
*/

namespace HTTP
{

class Response
{
  private:
	typedef std::map<std::string, std::string>::iterator _header_iterator;

	void initGeneralHeaderFields(void);
	void initResponseHeaderFields(void);
	void initEntityHeaderFields(void);
	void makeStatusLine(void);
	void makeHeader(void);
	void makeBody(void);
	void alignAutoIndex(size_t minus_len, int to_align);

	std::string _response;

	static const std::string _http_version;
	std::string _status_code;
	std::string _reason_phrase;
	Header _header;
	std::string _body;
	async::Logger &_logger;

	enum e_autoindex
	{
		AUTOINDEX_ALIGN_FILE_NAME,
		AUTOINDEX_ALIGN_FILE_SIZE
	};

  public:
	Response();
	Response(Response const &other);
	Response &operator=(Response const &other);
	~Response();

	std::string toString(void);

	// setter
	void setValue(const std::string &key, const std::string &val);

	void setDate(void);
	void setStatus(int status_code);
	void setContent(const std::string &content, const std::string &file_path);
	void setContentType(const std::string &file_path);
	void setContentLength(void);
	void setContentLength(size_t length);
	void setConnection(bool is_persistent);
	void setBody(const std::string &body);
	void setLocation(const std::string &uri);
	void makeDirectoryListing(const std::string &path, const std::string &uri);
};
} // namespace HTTP

#endif
