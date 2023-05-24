#ifndef CGI_REQUESTHANDLER_HPP
#define CGI_REQUESTHANDLER_HPP

#include "CGI/Request.hpp"
#include "CGI/Response.hpp"
#include "HTTP/Request.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/Logger.hpp"
#include "async/status.hpp"

namespace CGI
{
class RequestHandler
{
  private:
	async::FileReader *_reader;
	async::FileWriter *_writer;
	const Request &_request;
	Response _response;
	// std::string _cgi_path;
	int _status;

	int _pipe_fd[2];
	pid_t _pid;
	int _waitpid_status;

	// debug
	async::Logger &_logger;

	enum cgi_response_inner_status_e
	{
		CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN,
		CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN,
		CGI_RESPONSE_INNER_STATUS_READ_AGAIN,
		CGI_RESPONSE_INNER_STATUS_OK
	};

	int sendCGIRequest(void);
	int waitExecution(void);
	int makeCGIResponse(void);

  public:
	enum cgi_response_status_e
	{
		CGI_RESPONSE_STATUS_AGAIN,
		CGI_RESPONSE_STATUS_OK,
	};

	RequestHandler(const Request &http_request);
	~RequestHandler();

	int task(void);
	const CGI::Response &retrieve(void);
};
} // namespace CGI

#endif