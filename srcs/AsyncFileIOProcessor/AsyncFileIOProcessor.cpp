#include "AsyncFileIOProcessor.hpp"
#include <fcntl.h>

AsyncFileIOProcessor::AsyncFileIOProcessor(void) : _writer(0)
{
}

AsyncFileIOProcessor::AsyncFileIOProcessor(const AsyncFileIOProcessor &orig)
	: _writer(orig._writer)
{
}

AsyncFileIOProcessor &AsyncFileIOProcessor::operator=(
	const AsyncFileIOProcessor &orig)
{
	_writer = orig._writer;
	return (*this);
}

AsyncFileIOProcessor::AsyncFileIOProcessor(int fd)
	: _writer(fd), _status(LOAD_STATUS_AGAIN), _buffer("")
{
}

AsyncFileIOProcessor::AsyncFileIOProcessor(const std::string &path)
	: _writer(::open(path.c_str(), O_RDWR | O_CREAT,
					 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)),
	  _status(LOAD_STATUS_AGAIN), _buffer("")
{
}

AsyncFileIOProcessor::~AsyncFileIOProcessor()
{
}

std::string AsyncFileIOProcessor::retrieve(void)
{
	if (_status != LOAD_STATUS_OK)
		throw(
			std::logic_error("AsyncFileIOProcessor: File is not yet loaded."));
	return (_buffer);
}
