#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"
#include "utils/file.hpp"
#include "utils/string.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace async;
typedef unsigned long long ull_t;

bool FileIOProcessor::openFdByPath(int oflag)
{
	if (!_should_close)
		return (false);
	if (isDirectory(_path))
	{
		_status = status::ERROR_FILEISDIR;
		_error_msg = generateErrorMsgFileIsDir(_path);
		return (true);
	}
	int fd
		= ::open(_path.c_str(), oflag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0)
	{
		_status = status::ERROR_FILEOPENING;
		_error_msg = generateErrorMsgFileOpening(_path);
		return (true);
	}
	_fd = fd;
	return (false);
}

static unsigned int addTimeoutFromNow(unsigned int timeout_ms)
{
	if (timeout_ms == 0)
		return (0);
	ull_t result = (((ull_t)clock() * 1000)
					+ ((ull_t)timeout_ms * (ull_t)CLOCKS_PER_SEC))
				   / 1000;
	return (result);
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms, int fd)
	: _processor(NULL), _fd(fd), _path(""), _status(status::OK_BEGIN),
	  _buffer(""), _timeout_ms(addTimeoutFromNow(timeout_ms)),
	  _should_close(false)
{
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms,
								 const std::string &path)
	: _processor(NULL), _fd(-1), _path(path), _status(status::OK_BEGIN),
	  _buffer(""), _timeout_ms(addTimeoutFromNow(timeout_ms)),
	  _should_close(true)
{
}

FileIOProcessor::~FileIOProcessor()
{
	if (_should_close)
		close(_fd);
	delete _processor;
}

bool FileIOProcessor::checkTimeout(void)
{
	if (_timeout_ms == 0)
		return (false);
	if (clock() > _timeout_ms)
	{
		_status = status::ERROR_TIMEOUT;
		_error_msg = generateErrorMsgTimeout(_fd, _timeout_ms);
		return (true);
	}
	return (false);
}

std::string FileIOProcessor::retrieve(void)
{
	if (_status != status::OK_DONE)
		throw(std::logic_error("FileIOProcessor: File is not yet loaded."));
	return (_buffer);
}

const std::string &FileIOProcessor::errorMsg(void) const
{
	return (_error_msg);
}
