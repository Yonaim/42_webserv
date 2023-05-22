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

	checkTimeout();
	_writer.getReadBuf(_buffer);
	try
	{
		_writer.task();
	}
	catch (const IOProcessor::FileClosed &e)
	{
		_writer.getReadBuf(_buffer);
		_status = status::OK;
	}
	return (_status);
}
