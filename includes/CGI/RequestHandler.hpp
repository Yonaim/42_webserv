#ifndef CGI_REQUESTHANDLER_HPP
#define CGI_REQUESTHANDLER_HPP

#include "CGI/Request.hpp"
#include "CGI/Response.hpp"
#include "HTTP/Request.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"

namespace CGI
{
class RequestHandler
{
  private:
	async::FileReader *_reader;
	async::FileWriter *_writer;
	Request _request;
	Response _response;
	std::string _cgi_path;
	int _status;
	int _pipe_fd[2];

  public:
	enum response_status_e
	{
		CGI_RESPONSE_STATUS_OK = 0,
		CGI_RESPONSE_STATUS_AGAIN
	};

	RequestHandler(const HTTP::Request &http_request);
	~RequestHandler();

	int task(void);
	const CGI::Response &retrieve(void);
};
} // namespace CGI

#endif
