#include "async/FileIOProcessor.hpp"
#include "../../HTTP/utils.hpp" // TODO: 유틸 함수 하나로 정리
#include "async/JobStatus.hpp"
#include <fcntl.h>
#include <unistd.h>

using namespace async;
typedef unsigned long long ull_t;

static unsigned int addTimeoutFromNow(unsigned int timeout_ms)
{
	if (timeout_ms == 0)
		return (0);
	ull_t result = (((ull_t)clock() * 1000)
					+ ((ull_t)timeout_ms * (ull_t)CLOCKS_PER_SEC))
				   / 1000;
	return (result);
}

FileIOProcessor::FileIOProcessor(void) : _writer(0)
{
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms, int fd)
	: _writer(fd), _status(JobStatus::AGAIN), _buffer(""),
	  _timeout_ms(addTimeoutFromNow(timeout_ms)), _should_close(false)
{
}

FileIOProcessor::FileIOProcessor(unsigned int timeout_ms,
								 const std::string &path)
	: _writer(::open(path.c_str(), O_RDWR | O_CREAT,
					 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)),
	  _status(JobStatus::AGAIN), _buffer(""),
	  _timeout_ms(addTimeoutFromNow(timeout_ms)), _should_close(true)
{
}

FileIOProcessor &FileIOProcessor::operator=(const FileIOProcessor &orig)
{
	_writer = orig._writer;
	return (*this);
}

FileIOProcessor::FileIOProcessor(const FileIOProcessor &orig)
	: _writer(orig._writer)
{
}

FileIOProcessor::~FileIOProcessor()
{
	if (_should_close)
		close(_writer.getFd());
}

void FileIOProcessor::checkTimeout(void)
{
	if (_timeout_ms == 0)
		return;
	if (clock() > _timeout_ms)
		throw(FileIOProcessor::Timeout(_writer.getFd()));
}

std::string FileIOProcessor::retrieve(void)
{
	if (_status != JobStatus::OK)
		throw(std::logic_error("FileIOProcessor: File is not yet loaded."));
	return (_buffer);
}

FileIOProcessor::Timeout::Timeout(const int fd)
	: std::runtime_error("Timeout occured at " + toStr(fd))
{
}
