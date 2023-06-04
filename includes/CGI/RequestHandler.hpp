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
/**
 * @class RequestHandler
 * @brief CGI 요청을 처리하는 기본 클래스
 *
 * RequestHandler는 CGI 요청을 처리하기 위한 클래스이다. HTTP::Request와
 * CGI 경로를 입력 받아 CGI 요청을 생성하고 처리하고 CGI 응답을 생성한다.
 * FileReader와 FileWriter를 사용하여 파이프나 임시 정규 파일을 통해 입출력을
 * 처리하고 자식 프로세스를 생성하여 CGI를 실행한다.
 * 메시지 본문의 크기가 1024보다 작으면 파이프를, 그렇지 않으면 임시 정규 파일을
 * 통해 입출력을 처리한다.
 */
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
	async::Logger &_logger;

	char **getArgv(void);

  public:
	enum cgi_response_status_e
	{
		CGI_RESPONSE_STATUS_AGAIN,
		CGI_RESPONSE_STATUS_OK,
	};
	static const size_t pipeThreshold;

	RequestHandler(const Request &request, const std::string &exec_path);
	virtual ~RequestHandler();

	virtual int task(void) = 0;
	const Response &retrieve(void);
};

/**
 * @class RequestHandlerPipe
 * @brief 파이프를 사용하여 CGI 요청을 처리하는 클래스
 *
 * RequestHandlerPipe는 파이프를 사용하여 입출력을 처리하는 클래스이다.
 */
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

/**
 * @class RequestHandlerVnode
 * @brief 임시 정규 파일을 사용하여 CGI 요청을 처리하는 클래스
 *
 * RequestHandlerVnode는 임시 정규 파일을 사용하여 입출력을 처리하는 클래스이다.
 */
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
