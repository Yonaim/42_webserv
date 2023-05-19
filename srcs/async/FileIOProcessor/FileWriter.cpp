#include "async/FileIOProcessor.hpp"

using namespace async;

FileWriter::FileWriter() : FileIOProcessor()
{
}

FileWriter::FileWriter(const FileWriter &orig) : FileIOProcessor(orig)
{
}

FileWriter &FileWriter::operator=(const FileWriter &orig)
{
	FileIOProcessor::operator=(orig);
	return (*this);
}

FileWriter::FileWriter(int fd) : FileIOProcessor(fd)
{
}

FileWriter::FileWriter(const std::string &path, const std::string &content)
	: FileIOProcessor(path)
{
	_writer.setWriteBuf(content);
}

FileWriter::~FileWriter()
{
}

int FileWriter::task(void)
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
