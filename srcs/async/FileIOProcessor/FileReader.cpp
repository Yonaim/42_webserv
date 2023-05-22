#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"

using namespace async;

FileReader::FileReader() : FileIOProcessor()
{
}

FileReader::FileReader(const FileReader &orig) : FileIOProcessor(orig)
{
}

FileReader &FileReader::operator=(const FileReader &orig)
{
	FileIOProcessor::operator=(orig);
	return (*this);
}

FileReader::FileReader(unsigned int timeout_ms, int fd)
	: FileIOProcessor(timeout_ms, fd)
{
}

FileReader::FileReader(unsigned int timeout_ms, const std::string &path)
	: FileIOProcessor(timeout_ms, path)
{
}

FileReader::~FileReader()
{
}

int FileReader::task(void)
{
	if (_status == status::OK)
		return (_status);
	if (_status == status::BEGIN)
	{
		openFdByPath();
		_processor = new SingleIOProcessor(_fd);
		_status = status::AGAIN;
	}

	checkTimeout();
	_processor->getReadBuf(_buffer);
	try
	{
		_processor->task();
	}
	catch (const IOProcessor::FileClosed &e)
	{
		_processor->getReadBuf(_buffer);
		_status = status::OK;
	}
	return (_status);
}
