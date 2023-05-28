#include "CGI/RequestHandler.hpp"
#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;

// TODO:
// 추후에 status_code 수정 필요함
#define ERROR_CODE 11

RequestHandler::RequestHandler(const Request &request,
							   const unsigned int timeout_ms)
	: _reader(NULL), _writer(NULL), _request(request),
	  _status(CGI_RESPONSE_INNER_STATUS_BEGIN), _pid(-1),
	  _waitpid_status(-1),
	  _logger(async::Logger::getLogger("CGIRequestHandler"))
{
	try
	{
		if (::pipe(_read_pipe_fd) < 0 || ::pipe(_write_pipe_fd) < 0)
		{
			// TODO: 예외 처리
			throw(std::runtime_error("Failed to create pipe."));
		}
		_logger << "read_pipe[0]: " << _read_pipe_fd[0] << async::debug;
		_logger << "read_pipe[1]: " << _read_pipe_fd[1] << async::debug;
		_logger << "write_pipe[0]: " << _write_pipe_fd[0] << async::debug;
		_logger << "write_pipe[1]: " << _write_pipe_fd[1] << async::debug;

		_writer = new async::FileWriter(timeout_ms, _write_pipe_fd[1],
										_request.getMessageBody());
		_reader = new async::FileReader(timeout_ms, _read_pipe_fd[0]);
	}
	catch (const std::exception &e)
	{
		_logger << e.what() << async::debug;
		delete _writer;
		_writer = NULL;
		delete _reader;
		_reader = NULL;
		throw(std::runtime_error("Failed to create RequestHandler"));
	}
}

RequestHandler::~RequestHandler()
{
	close(_read_pipe_fd[0]);
	close(_read_pipe_fd[1]);
	delete _writer;
	delete _reader;
}

int RequestHandler::fork()
{
	_pid = ::fork();
	if (_pid < 0)
	{
		_logger << "failed to fork" << async::debug;
		_response.setError(ERROR_CODE);
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
	else if (_pid == 0)
	{
		if (::dup2(_write_pipe_fd[0], STDIN_FILENO) < 0
			|| ::dup2(_read_pipe_fd[1], STDOUT_FILENO) < 0)
		{
			_logger << "failed to dup2" << async::debug;
			// TODO:
			// exit status 뭐로 할지 결정할것
			std::exit(CGI_RESPONSE_INNER_STATUS_OK);
		}
		::close(_write_pipe_fd[0]);
		::close(_write_pipe_fd[1]);
		::close(_read_pipe_fd[0]);
		::close(_read_pipe_fd[1]);
		// TODO:
		// CGIRequest에서 getPath, getEnv 내부 메소드 필요할듯
		// TODO:
		// CGI 인자로 넘길 것 결정하기(NULL 말고)
		execve(_request.getPath().c_str(), NULL, _request.getEnv());
		_logger << "failed to execve cgi program" << async::debug;
		std::exit(CGI_RESPONSE_INNER_STATUS_OK);
	}
	else
	{
		::close(_write_pipe_fd[0]);
		::close(_read_pipe_fd[1]);
		_status = CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandler::sendCGIRequest()
{
	int rc = _writer->task();

	switch (rc)
	{
	case async::status::OK:
		close(_write_pipe_fd[1]);
		delete _writer;
		_writer = NULL;
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	case async::status::AGAIN:
		_status = CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	default:
		_response.setError(ERROR_CODE);
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
}

int RequestHandler::waitExecution()
{
	int rc = ::waitpid(_pid, &_waitpid_status, WNOHANG);

	if (rc < 0)
	{
		_response.setError(ERROR_CODE);
		_status = CGI_RESPONSE_INNER_STATUS_OK;
	}
	else if (rc == 0)
	{
		_logger << "waiting child" << async::debug;
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
	}
	else
	{
		// TODO: _waitpid_status 값 체크하기
		_logger << "waitpid done" << async::debug;
		_status = CGI_RESPONSE_INNER_STATUS_READ_AGAIN;
	}

	return ((_status == CGI_RESPONSE_INNER_STATUS_OK)
				? CGI_RESPONSE_STATUS_OK
				: CGI_RESPONSE_STATUS_AGAIN);
};

int RequestHandler::makeCGIResponse()
{
	int read_status = _reader->task();

	switch (read_status)
	{
	case async::status::OK:
		// TODO:
		//  CGI/Response makeResponse 멤버 함수 구현
		_logger << "read_status is ok: " << read_status << async::debug;
		_logger << _reader->retrieve() << async::debug;
		_response.makeResponse(_reader->retrieve());
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	case async::status::AGAIN:
		_logger << "read_status is again: " << read_status << async::debug;
		_status = CGI_RESPONSE_INNER_STATUS_READ_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	default:
		_logger << "read_status is error: " << read_status << async::debug;
		_response.setError(ERROR_CODE);
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
};

int RequestHandler::task(void)
{
	_logger << "status : " << _status << async::debug;
	switch (_status)
	{
	case CGI_RESPONSE_INNER_STATUS_BEGIN:
		return (fork());
	case CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN:
		return (sendCGIRequest());
	case CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN:
		return (waitExecution());
	case CGI_RESPONSE_INNER_STATUS_READ_AGAIN:
		return (makeCGIResponse());
	default:
		return (CGI_RESPONSE_STATUS_OK);
	}
}

const CGI::Response &RequestHandler::retrieve(void)
{
	if (_status != CGI_RESPONSE_INNER_STATUS_OK)
		throw(std::runtime_error("CGI Response has not been made yet."));
	return (_response);
}
