#include "CGI/RequestHandler.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;

void RequestHandlerPipe::closePipe(int &fd)
{
	if (fd == -1)
		return;
	::close(fd);
	fd = -1;
}

void RequestHandlerPipe::closeAllPipes(void)
{
	closePipe(_read_pipe_fd[0]);
	closePipe(_read_pipe_fd[1]);
	closePipe(_write_pipe_fd[0]);
	closePipe(_write_pipe_fd[1]);
}

RequestHandlerPipe::RequestHandlerPipe(const Request &request,
									   const std::string &exec_path,
									   const unsigned int timeout_ms)
	: RequestHandler(request, exec_path, timeout_ms)
{
	_read_pipe_fd[0] = -1, _read_pipe_fd[1] = -1;
	_write_pipe_fd[0] = -1, _write_pipe_fd[1] = -1;
	if (::pipe(_read_pipe_fd) < 0 || ::pipe(_write_pipe_fd) < 0)
	{
		closeAllPipes();
		throw(std::runtime_error("Constructor: Failed to create pipe."));
	}

	LOG_DEBUG("read_pipe[0]: " << _read_pipe_fd[0]);
	LOG_DEBUG("read_pipe[1]: " << _read_pipe_fd[1]);
	LOG_DEBUG("write_pipe[0]: " << _write_pipe_fd[0]);
	LOG_DEBUG("write_pipe[1]: " << _write_pipe_fd[1]);

	if (_request.getMessageBody().length() > 0)
		_writer = new async::FileWriter(timeout_ms, _write_pipe_fd[1],
										_request.getMessageBody());
	_reader = new async::FileReader(timeout_ms, _read_pipe_fd[0], true);
}

RequestHandlerPipe::~RequestHandlerPipe()
{
	closeAllPipes();
	delete _writer;
	delete _reader;
}

int RequestHandlerPipe::fork()
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
			LOG_DEBUG("failed to dup2");
			std::exit(2);
		}
		closePipe(_write_pipe_fd[0]);
		closePipe(_write_pipe_fd[1]);
		closePipe(_read_pipe_fd[0]);
		closePipe(_read_pipe_fd[1]);
		LOG_DEBUG("calling execve(" << _exec_path.c_str() << "), good bye!");
		execve(_exec_path.c_str(), getArgv(), _request.getEnv());
		std::exit(2);
	}
	else
	{
		LOG_DEBUG("successed to fork.");
		closePipe(_write_pipe_fd[0]);
		closePipe(_read_pipe_fd[1]);

		_status = CGI_RESPONSE_INNER_STATUS_RW_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandlerPipe::waitRWOperation()
{
	if (_writer)
	{
		switch (_writer->task())
		{
		case async::status::OK_DONE:
			LOG_DEBUG("successed to send CGI request");
			closePipe(_write_pipe_fd[1]);
			delete _writer;
			_writer = NULL;
			break;
		case async::status::OK_AGAIN:
			LOG_DEBUG("writing CGI request");
			break;
		case async::status::ERROR_TIMEOUT:
			throw(std::runtime_error(
				"Timeout occured while writing CGI Request"));
		default:
			throw(std::runtime_error("failed to send CGI Request"));
		}
	}

	if (_reader)
	{
		switch (_reader->task())
		{
		case async::status::OK_DONE: {
			LOG_DEBUG("read status is ok");
			LOG_DEBUG("buffer: " << _reader->retrieve());

			std::string cgi_output = _reader->retrieve();
			closePipe(_read_pipe_fd[0]);
			_response.makeResponse(cgi_output);
			delete _reader;
			_reader = NULL;
			break;
		}
		case async::status::OK_AGAIN: {
			LOG_DEBUG("read status is again");
			break;
		}
		case async::status::ERROR_TIMEOUT:
			throw(std::runtime_error(
				"Timeout occured while reading from CGI Request"));
		default:
			throw(std::runtime_error("failed to receive CGI response"));
		}
	}

	if (!_writer && !_reader)
	{
		setTimeout();
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
	}

	return (CGI_RESPONSE_STATUS_AGAIN);
}

int RequestHandlerPipe::waitExecution()
{
	int rc = ::waitpid(_pid, &_waitpid_status, WNOHANG);

	if (rc < 0)
	{
		throw(std::runtime_error("failed to waitpid"));
	}
	else if (rc == 0)
	{
		LOG_DEBUG("waiting child");
		checkTimeout();
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	else
	{
		if ((WIFEXITED(_waitpid_status) && (WEXITSTATUS(_waitpid_status) == 2))
			|| WIFSIGNALED(_waitpid_status))
			throw(std::runtime_error("CGI execution failed"));
		LOG_DEBUG("child process done");
		LOG_DEBUG("successed CGI execution");
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
}

int RequestHandlerPipe::task(void)
{
	LOG_DEBUG("status : " << _status);
	switch (_status)
	{
	case CGI_RESPONSE_INNER_STATUS_BEGIN:
		return (fork());
	case CGI_RESPONSE_INNER_STATUS_RW_AGAIN:
		return (waitRWOperation());
	case CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN:
		return (waitExecution());
	default:
		return (CGI_RESPONSE_STATUS_OK);
	}
}
