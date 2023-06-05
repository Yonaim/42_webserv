#include "async/FileIOHandler.hpp"
#include "async/status.hpp"
#include "utils/file.hpp"
#include "utils/string.hpp"
#include <unistd.h>

using namespace async;
typedef unsigned long long ull_t;

bool FileIOHandler::openFdByPath(const char *mode)
{
	if (!_should_close)
		return (false);
	if (isDirectory(_path))
	{
		_status = status::ERROR_FILEISDIR;
		_error_msg = generateErrorMsgFileIsDir(_path);
		return (true);
	}
	_stream = fopen(_path.c_str(), mode);
	if (!_stream)
	{
		_status = status::ERROR_FILEOPENING;
		_error_msg = generateErrorMsgFileOpening(_path);
		return (true);
	}
	_fd = fileno(_stream);
	return (false);
}

static clock_t msToClockT(const unsigned int ms)
{
	ull_t result = (((ull_t)ms) * CLOCKS_PER_SEC) / 1000;
	return (result);
}

FileIOHandler::FileIOHandler(unsigned int timeout_ms, int fd)
	: _processor(NULL), _stream(NULL), _fd(fd), _path(""),
	  _status(status::OK_BEGIN), _buffer(""), _timeout(msToClockT(timeout_ms)),
	  _next_timeout(clock() + _timeout), _should_close(false)
{
}

FileIOHandler::FileIOHandler(unsigned int timeout_ms, const std::string &path)
	: _processor(NULL), _stream(NULL), _fd(-1), _path(path),
	  _status(status::OK_BEGIN), _buffer(""), _timeout(msToClockT(timeout_ms)),
	  _next_timeout(clock() + _timeout), _should_close(true)
{
}

FileIOHandler::~FileIOHandler()
{
	if (_should_close && _stream)
		fclose(_stream);
}

void FileIOHandler::renewTimeout(void)
{
	_next_timeout += _timeout;
}

bool FileIOHandler::checkTimeout(void)
{
	if (_timeout == 0)
		return (false);
	if (clock() > _next_timeout)
	{
		_status = status::ERROR_TIMEOUT;
		_error_msg = generateErrorMsgTimeout(_fd, _timeout);
		return (true);
	}
	return (false);
}

std::string FileIOHandler::retrieve(void)
{
	if (_status != status::OK_DONE)
		throw(std::logic_error("FileIOHandler: File is not yet loaded."));
	return (_buffer);
}

const std::string &FileIOHandler::errorMsg(void) const
{
	return (_error_msg);
}
