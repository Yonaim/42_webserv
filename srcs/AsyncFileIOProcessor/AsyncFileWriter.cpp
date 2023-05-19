#include "AsyncFileIOProcessor.hpp"

AsyncFileWriter::AsyncFileWriter() : AsyncFileIOProcessor()
{
}

AsyncFileWriter::AsyncFileWriter(const AsyncFileWriter &orig)
	: AsyncFileIOProcessor(orig)
{
}

AsyncFileWriter &AsyncFileWriter::operator=(const AsyncFileWriter &orig)
{
	AsyncFileIOProcessor::operator=(orig);
	return (*this);
}

AsyncFileWriter::AsyncFileWriter(int fd) : AsyncFileIOProcessor(fd)
{
}

AsyncFileWriter::AsyncFileWriter(const std::string &path,
								 const std::string &content)
	: AsyncFileIOProcessor(path)
{
	_writer.setWriteBuf(content);
}

AsyncFileWriter::~AsyncFileWriter()
{
}

int AsyncFileWriter::task(void)
{
	if (_status == LOAD_STATUS_OK)
		return (LOAD_STATUS_OK);
	_writer.task();
	if (_writer.writeDone())
	{
		_status = LOAD_STATUS_OK;
		return (LOAD_STATUS_OK);
	}
	return (LOAD_STATUS_AGAIN);
}
