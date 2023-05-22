#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"
#include "utils/string.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace async;
typedef unsigned long long ull_t;

void FileIOProcessor::openFdByPath(int oflag)
{
	if (!_should_close)
		return;
	int fd
		= ::open(_path.c_str(), oflag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0)
		throw(FileIOProcessor::FileOpeningError(_path, strerror(errno)));
	_fd = fd;
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

FileIOProcessor::FileIOProcessor(void)
	: _processor(NULL), _timeout_ms(0), _should_close(false)
{
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms, int fd)
	: _processor(NULL), _fd(fd), _path(""), _status(status::BEGIN), _buffer(""),
	  _timeout_ms(addTimeoutFromNow(timeout_ms)), _should_close(false)
{
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms,
								 const std::string &path)
	: _processor(NULL), _fd(-1), _path(path), _status(status::BEGIN),
	  _buffer(""), _timeout_ms(addTimeoutFromNow(timeout_ms)),
	  _should_close(true)
{
}

FileIOProcessor &FileIOProcessor::operator=(const FileIOProcessor &orig)
{
	_processor = orig._processor;
	_fd = orig._fd;
	_status = orig._status;
	_buffer = orig._buffer;
	return (*this);
}

FileIOProcessor::FileIOProcessor(const FileIOProcessor &orig)
	: _processor(orig._processor), _fd(orig._fd), _path(orig._path),
	  _status(orig._status), _buffer(orig._buffer),
	  _timeout_ms(orig._timeout_ms), _should_close(orig._should_close)
{
}

FileIOProcessor::~FileIOProcessor()
{
	if (_should_close)
		close(_fd);
	delete _processor;
}

void FileIOProcessor::checkTimeout(void)
{
	if (_timeout_ms == 0)
		return;
	if (clock() > _timeout_ms)
		throw(FileIOProcessor::Timeout(_fd));
}

std::string FileIOProcessor::retrieve(void)
{
	if (_status != status::OK)
		throw(std::logic_error("FileIOProcessor: File is not yet loaded."));
	return (_buffer);
}

FileIOProcessor::Timeout::Timeout(const int fd)
	: std::runtime_error("Timeout occured at " + toStr(fd))
{
}

FileIOProcessor::FileOpeningError::FileOpeningError(const std::string &path,
													const std::string &cause)
	: std::runtime_error(std::string("Error while opening file \"") + path
						 + "\": " + cause)
{
}
