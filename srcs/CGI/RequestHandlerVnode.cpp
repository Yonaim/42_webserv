#include "CGI/RequestHandler.hpp"
#include "utils/hash.hpp"
#include "utils/string.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;

RequestHandlerVnode::RequestHandlerVnode(const Request &request,
										 const std::string &exec_path,
										 const unsigned int timeout_ms,
										 const std::string &temp_dir_path)
	: RequestHandler(request, exec_path),
	  _input_file_path(temp_dir_path + "/" + generateHash(toStr(clock()))),
	  _output_file_path(temp_dir_path + "/" + generateHash(toStr(clock()))),
	  _timeout_ms(timeout_ms)
{
	_writer = new async::FileWriter(_timeout_ms, _input_file_path,
									_request.getMessageBody());
	_logger << async::verbose << "CGI input file: " << _input_file_path;
	_logger << async::verbose << "CGI output file: " << _output_file_path;
}

RequestHandlerVnode::~RequestHandlerVnode()
{
	if (unlink(_input_file_path.c_str()) == -1)
		_logger << async::error
				<< "Failed to delete file: " << _input_file_path;
	if (unlink(_output_file_path.c_str()) == -1)
		_logger << async::error
				<< "Failed to delete file: " << _output_file_path;
	delete _writer;
	delete _reader;
}

int RequestHandlerVnode::waitWriteInputOperation(void)
{
	switch (_writer->task())
	{
	case async::status::OK_DONE:
		ASYNC_LOG_DEBUG(_logger, "Finished writing CGI Request body to file");
		delete _writer;
		_writer = NULL;
		_status = CGI_RESPONSE_INNER_STATUS_FORK_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	case async::status::OK_AGAIN:
		ASYNC_LOG_DEBUG(_logger, "writing CGI Request body");
		return (CGI_RESPONSE_STATUS_AGAIN);
	default:
		throw(std::runtime_error("failed to write CGI Request"));
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandlerVnode::fork()
{
	_pid = ::fork();
	if (_pid < 0)
	{
		throw(std::runtime_error("failed to fork"));
	}
	else if (_pid == 0)
	{
		int input_fd = ::open(_input_file_path.c_str(), O_RDONLY,
							  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		int output_fd = ::open(_output_file_path.c_str(), O_WRONLY | O_CREAT,
							   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (input_fd < 0 || output_fd < 0)
		{
			ASYNC_LOG_DEBUG(_logger, "failed to dup2");
			std::exit(2);
		}
		if (::dup2(input_fd, STDIN_FILENO) < 0
			|| ::dup2(output_fd, STDOUT_FILENO) < 0)
		{
			ASYNC_LOG_DEBUG(_logger, "failed to dup2");
			std::exit(2);
		}
		close(input_fd);
		close(output_fd);
		ASYNC_LOG_DEBUG(_logger, "calling execve(" << _exec_path.c_str()
												   << "), good bye!");
		/* TODO: (해야 한다면) argv 만들어 입력 (아닐수도 있고)
		 * 근거: Your program should call the CGI with the file requested as
		 * first argument.*/
		execve(_exec_path.c_str(), NULL, _request.getEnv());
		std::exit(2);
	}
	else
	{
		ASYNC_LOG_DEBUG(_logger, "successed to fork.");
		_status = CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandlerVnode::waitExecution()
{
	int rc = ::waitpid(_pid, &_waitpid_status, WNOHANG);

	if (rc < 0)
	{
		throw(std::runtime_error("failed to waitpid"));
	}
	else if (rc == 0)
	{
		ASYNC_LOG_DEBUG(_logger, "waiting child");
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	else
	{
		if ((WIFEXITED(_waitpid_status) && (WEXITSTATUS(_waitpid_status) == 2))
			|| WIFSIGNALED(_waitpid_status))
			throw(std::runtime_error("CGI execution failed"));
		ASYNC_LOG_DEBUG(_logger, "child process done");
		ASYNC_LOG_DEBUG(_logger, "successed CGI execution");
		_status = CGI_RESPONSE_INNER_STATUS_READ_AGAIN;
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
}

int RequestHandlerVnode::waitReadOutputOperation(void)
{
	switch (_reader->task())
	{
	case async::status::OK_DONE: {
		ASYNC_LOG_DEBUG(_logger, "read status is ok");
		ASYNC_LOG_DEBUG(_logger, "buffer: " << _reader->retrieve());
		std::string cgi_output = _reader->retrieve();
		_response.makeResponse(cgi_output);
		delete _reader;
		_reader = NULL;
		_status = CGI_RESPONSE_INNER_STATUS_OK;
		return (CGI_RESPONSE_STATUS_OK);
	}
	case async::status::OK_AGAIN: {
		ASYNC_LOG_DEBUG(_logger, "read status is again");
		ASYNC_LOG_DEBUG(_logger, "read from file " << _output_file_path);
		return (CGI_RESPONSE_STATUS_AGAIN);
	}
	default:
		throw(std::runtime_error("failed to receive CGI response"));
	}
	return (CGI_RESPONSE_STATUS_OK);
}

int RequestHandlerVnode::task(void)
{
	ASYNC_LOG_DEBUG(_logger, "status : " << _status);
	try
	{
		switch (_status)
		{
		case CGI_RESPONSE_INNER_STATUS_BEGIN:
			return (waitWriteInputOperation());
		case CGI_RESPONSE_INNER_STATUS_FORK_AGAIN:
			return (fork());
		case CGI_RESPONSE_INNER_STATUS_WAITPID_AGAIN:
			return (waitExecution());
		case CGI_RESPONSE_INNER_STATUS_READ_AGAIN:
			_reader = new async::FileReader(_timeout_ms, _output_file_path);
			return (waitReadOutputOperation());
		default:
			return (CGI_RESPONSE_STATUS_OK);
		}
	}
	catch (const std::runtime_error &e)
	{
		ASYNC_LOG_DEBUG(_logger, e.what());
		throw(e);
	}
}
