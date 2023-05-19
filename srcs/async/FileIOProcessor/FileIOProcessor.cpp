#include "async/FileIOProcessor.hpp"
#include <fcntl.h>

using namespace async;

FileIOProcessor::FileIOProcessor(void) : _writer(0)
{
}

FileIOProcessor::FileIOProcessor(const FileIOProcessor &orig)
	: _writer(orig._writer)
{
}

FileIOProcessor &FileIOProcessor::operator=(const FileIOProcessor &orig)
{
	_writer = orig._writer;
	return (*this);
}

FileIOProcessor::FileIOProcessor(int fd)
	: _writer(fd), _status(LOAD_STATUS_AGAIN), _buffer("")
{
}

FileIOProcessor::FileIOProcessor(const std::string &path)
	: _writer(::open(path.c_str(), O_RDWR | O_CREAT,
					 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)),
	  _status(LOAD_STATUS_AGAIN), _buffer("")
{
}

FileIOProcessor::~FileIOProcessor()
{
}

std::string FileIOProcessor::retrieve(void)
{
	if (_status != LOAD_STATUS_OK)
		throw(std::logic_error("FileIOProcessor: File is not yet loaded."));
	return (_buffer);
}
