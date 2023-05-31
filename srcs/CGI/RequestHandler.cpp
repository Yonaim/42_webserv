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
							   const std::string &exec_path,
							   const unsigned int timeout_ms)
	: _request(request), _reader(NULL), _writer(NULL), _exec_path(exec_path),
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

		if (_request.getMessageBody().length() > 0)
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
		_logger << async::verbose << "calling execve(" << _exec_path.c_str()
				<< "), good bye!";
		/* TODO: (해야 한다면) argv 만들어 입력 (아닐수도 있고)
		 * 근거: Your program should call the CGI with the file requested as
		 * first argument.*/
		execve(_exec_path.c_str(), NULL, _request.getEnv());
		std::exit(2);
	}
	else
	{
		_logger << async::debug << "successed to fork.";
		closePipe(_write_pipe_fd[0]);
		closePipe(_read_pipe_fd[1]);
		if (_request.getMessageBody().length() > 0)
			_status = CGI_RESPONSE_INNER_STATUS_WRITE_AGAIN;
		else
			_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandler::sendCGIRequestBody()
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
		if ((WIFEXITED(_waitpid_status) && (WEXITSTATUS(_waitpid_status) == 2))
			|| WIFSIGNALED(_waitpid_status))
			throw(std::runtime_error("CGI execution failed"));
		_logger << async::debug << "child process done";
		_status = CGI_RESPONSE_INNER_STATUS_READ_AGAIN;
	}

	return ((_status == CGI_RESPONSE_INNER_STATUS_OK)
				? CGI_RESPONSE_STATUS_OK
				: CGI_RESPONSE_STATUS_AGAIN);
}

int RequestHandler::makeCGIResponse()
{
	int rc = _reader->task();

	switch (rc)
	{
	case async::status::OK: {
		_logger << async::debug << "read_status is ok: " << rc;
		_logger << async::debug << "buffer: " << _reader->retrieve();

		std::string cgi_output = _reader->retrieve();
		closePipe(_read_pipe_fd[0]);
		_response.makeResponse(cgi_output);
		delete _reader;
		_reader = NULL;
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
	case async::status::AGAIN: {
		_logger << async::debug << "read_status is again: " << rc;

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
			return (sendCGIRequestBody());
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
