#include "CGI/RequestHandler.hpp"
#include <unistd.h>

using namespace CGI;

RequestHandler::RequestHandler(const Request &http_request)
	: _request(http_request)
{
}

RequestHandler::~RequestHandler()
{
}

int RequestHandler::task(void)
{
	if (pipe(_pipe_fd) < 0)
	{
		// TODO: 예외처리
	}
	// pipe에 request의 message body 작성하기
	_writer
		= new async::FileWriter(1000, _pipe_fd[1], _request.getMessageBody());
	// fork 통해서 CGI 프로그램 실행시키기
	int pid = fork();
	if (pid == 0)
	{
		// dup2로 표준 입력과 표준 출력을 파이프로 바꾸기
		// execve();
	}
	else if (pid > 0)
	{
		waitpid(pid, NULL, 0);
	}
	// pipe에서 response 읽고 가공하기
	_reader = new async::FileReader(1000, _pipe_fd[0]);
	return (CGI_RESPONSE_STATUS_OK);
}

const CGI::Response &RequestHandler::retrieve(void)
{
	if (_status != CGI_RESPONSE_STATUS_OK)
		throw(std::runtime_error("CGI Response has not been made yet."));
	return (_response);
}
