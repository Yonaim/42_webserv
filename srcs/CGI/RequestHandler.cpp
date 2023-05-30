#include "CGI/RequestHandler.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;

void RequestHandler::closePipe(int &fd)
{
	if (fd == -1)
		return;
	::close(fd);
	fd = -1;
}

void RequestHandler::closeAllPipes(void)
{
	closePipe(_read_pipe_fd[0]);
	closePipe(_read_pipe_fd[1]);
	closePipe(_write_pipe_fd[0]);
	closePipe(_write_pipe_fd[1]);
}

RequestHandler::RequestHandler(const Request &request,
							   const unsigned int timeout_ms)
	: _reader(NULL), _writer(NULL), _request(request),
	  _status(CGI_RESPONSE_INNER_STATUS_BEGIN), _pid(-1), _waitpid_status(-1),
	  _logger(async::Logger::getLogger("CGIRequestHandler"))
{
	try
	{
		_read_pipe_fd[0] = -1, _read_pipe_fd[1] = -1;
		_write_pipe_fd[0] = -1, _write_pipe_fd[1] = -1;

		if (::pipe(_read_pipe_fd) < 0 || ::pipe(_write_pipe_fd) < 0)
			throw(std::runtime_error("Constructor: Failed to create pipe."));

		_logger << async::debug << "read_pipe[0]: " << _read_pipe_fd[0];
		_logger << async::debug << "read_pipe[1]: " << _read_pipe_fd[1];
		_logger << async::debug << "write_pipe[0]: " << _write_pipe_fd[0];
		_logger << async::debug << "write_pipe[1]: " << _write_pipe_fd[1];

		_writer = new async::FileWriter(timeout_ms, _write_pipe_fd[1],
										_request.getMessageBody());
		_reader = new async::FileReader(timeout_ms, _read_pipe_fd[0]);
	}
	catch (const std::runtime_error &e)
	{
		closeAllPipes();
		_logger << async::debug << e.what();
		throw(e);
	}
}

RequestHandler::~RequestHandler()
{
	closeAllPipes();
	delete _writer;
	delete _reader;
}

int RequestHandler::fork()
{
	_pid = ::fork();
	if (_pid < 0)
	{
		throw(std::runtime_error("failed to fork"));
	}
	else if (_pid == 0)
	{
		if (::dup2(_write_pipe_fd[0], STDIN_FILENO) < 0
			|| ::dup2(_read_pipe_fd[1], STDOUT_FILENO) < 0)
		{
			_logger << async::debug << "failed to dup2";
			std::exit(2);
		}
		closePipe(_write_pipe_fd[0]);
		closePipe(_write_pipe_fd[1]);
		closePipe(_read_pipe_fd[0]);
		closePipe(_read_pipe_fd[1]);
		execve(_request.getPath().c_str(), NULL, _request.getEnv());
		std::exit(2);
	}
	else
	{
		_logger << async::debug << "successed to fork.";
		closePipe(_write_pipe_fd[0]);
		closePipe(_read_pipe_fd[1]);
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
		_logger << async::debug << "successed to write CGI request";
		closePipe(_write_pipe_fd[1]);
		delete _writer;
		_writer = NULL;
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	case async::status::AGAIN:
		_logger << async::debug << "writing CGI request";
		_status = CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	default:
		throw(std::runtime_error("failed to send CGI Request"));
	}
}

int RequestHandler::waitExecution()
{
	int rc = ::waitpid(_pid, &_waitpid_status, WNOHANG);

	if (rc < 0)
	{
		throw(std::runtime_error("failed to waitpid"));
	}
	else if (rc == 0)
	{
		_logger << async::debug << "waiting child";
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
	}
	else
	{
		// TODO: _waitpid_status 값 체크하기
		_logger << async::debug << "child process done";
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
		_logger << async::debug << "read_status is ok: " << read_status;
		_logger << async::debug << _reader->retrieve();
		_response.makeResponse(_reader->retrieve());
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	case async::status::AGAIN:
		_logger << async::debug << "read_status is again: " << read_status;
		_status = CGI_RESPONSE_INNER_STATUS_READ_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	default:
		throw(std::runtime_error("failed to read CGI response"));
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandler::task(void)
{
	_logger << async::debug << "status : " << _status;
	try
	{
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
	catch (const std::runtime_error &e)
	{
		closeAllPipes();
		_logger << async::debug << e.what();
		throw(e);
	}
}

const CGI::Response &RequestHandler::retrieve(void)
{
	if (_status != CGI_RESPONSE_INNER_STATUS_OK)
		throw(std::runtime_error("CGI Response has not been made yet."));
	return (_response);
}
