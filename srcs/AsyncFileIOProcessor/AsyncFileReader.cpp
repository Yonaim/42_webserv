#include "AsyncFileIOProcessor.hpp"

AsyncFileReader::AsyncFileReader() : AsyncFileIOProcessor()
{
}

AsyncFileReader::AsyncFileReader(const AsyncFileReader &orig)
	: AsyncFileIOProcessor(orig)
{
}

AsyncFileReader &AsyncFileReader::operator=(const AsyncFileReader &orig)
{
	AsyncFileIOProcessor::operator=(orig);
	return (*this);
}

AsyncFileReader::AsyncFileReader(int fd) : AsyncFileIOProcessor(fd)
{
}

AsyncFileReader::AsyncFileReader(const std::string &path)
	: AsyncFileIOProcessor(path)
{
}

AsyncFileReader::~AsyncFileReader()
{
}

int AsyncFileReader::task(void)
{
	if (_status == LOAD_STATUS_OK)
		return (LOAD_STATUS_OK);
	_writer.getReadBuf(_buffer);
	try
	{
		_writer.task();
	}
	catch (const AsyncIOProcessor::FileClosed &e)
	{
		_writer.getReadBuf(_buffer);
		_status = LOAD_STATUS_OK;
		return (LOAD_STATUS_OK);
	}
	return (LOAD_STATUS_AGAIN);
}
