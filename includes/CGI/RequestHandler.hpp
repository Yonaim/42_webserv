#ifndef CGI_REQUESTHANDLER_HPP
#define CGI_REQUESTHANDLER_HPP

#include "CGI/Request.hpp"
#include "CGI/Response.hpp"
#include "HTTP/Request.hpp"
#include "async/FileIOHandler.hpp"
#include "async/Logger.hpp"
#include "async/status.hpp"

namespace CGI
{
class RequestHandler
{
  protected:
	enum cgi_response_inner_status_e
	{
		CGI_RESPONSE_INNER_STATUS_BEGIN,
		CGI_RESPONSE_INNER_STATUS_RW_AGAIN,
		CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN,
		CGI_RESPONSE_INNER_STATUS_READ_AGAIN,
		CGI_RESPONSE_INNER_STATUS_FORK_AGAIN,
		CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN,
		CGI_RESPONSE_INNER_STATUS_OK
	};

	const Request _request;
	Response _response;
	async::FileReader *_reader;
	async::FileWriter *_writer;
	std::string _exec_path;
	pid_t _pid;
	int _waitpid_status;
	int _status;
	unsigned int _timeout_ms;
	clock_t _timeout;
	async::Logger &_logger;

	char **getArgv(void);
	void setTimeout(void);
	bool checkTimeout(void);

  public:
	enum cgi_response_status_e
	{
		CGI_RESPONSE_STATUS_AGAIN,
		CGI_RESPONSE_STATUS_OK,
	};
	static const size_t pipeThreshold;

	RequestHandler(const Request &request, const std::string &exec_path,
				   unsigned int timeout_ms);
	virtual ~RequestHandler();

	virtual int task(void) = 0;
	const Response &retrieve(void);
};

class RequestHandlerPipe : public RequestHandler
{
  private:
	int _read_pipe_fd[2];
	int _write_pipe_fd[2];

	void closePipe(int &fd);
	int fork(void);
	int waitRWOperation(void);
	int waitExecution(void);
	void closeAllPipes(void);

  public:
	RequestHandlerPipe(const Request &request, const std::string &exec_path,
					   const unsigned int timeout_ms);
	virtual ~RequestHandlerPipe();

	virtual int task(void);
};

class RequestHandlerVnode : public RequestHandler
{
  private:
	std::string _input_file_path;
	std::string _output_file_path;
	unsigned int _timeout_ms;

	int waitWriteInputOperation(void);
	int fork(void);
	int waitExecution(void);
	int waitReadOutputOperation(void);

  public:
	RequestHandlerVnode(const Request &request, const std::string &exec_path,
						const unsigned int timeout_ms,
						const std::string &temp_dir_path);
	virtual ~RequestHandlerVnode();

	virtual int task(void);
};
} // namespace CGI

#endif
