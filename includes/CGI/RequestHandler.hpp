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
	const Request _request;
	Response _response;
	async::FileReader *_reader;
	async::FileWriter *_writer;
	std::string _exec_path;
	int _status;

	int _read_pipe_fd[2];
	int _write_pipe_fd[2];
	pid_t _pid;
	int _waitpid_status;

	// debug
	async::Logger &_logger;

	enum cgi_response_inner_status_e
	{
		CGI_RESPONSE_INNER_STATUS_BEGIN,
		CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN,
		CGI_RESPONSE_INNER_STATUS_READ_AGAIN,
		CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN,
		CGI_RESPONSE_INNER_STATUS_OK
	};

	void closePipe(int &fd);
	int fork(void);
	int sendCGIRequestBody(void);
	int waitExecution(void);
	int receiveCGIResponse(void);
	void closeAllPipes(void);

  public:
	enum cgi_response_status_e
	{
		CGI_RESPONSE_STATUS_AGAIN,
		CGI_RESPONSE_STATUS_OK,
	};

	RequestHandler(const Request &request, const std::string &exec_path,
				   const unsigned int timeout_ms);
	~RequestHandler();

	int task(void);
	const Response &retrieve(void);
};
} // namespace CGI

#endif