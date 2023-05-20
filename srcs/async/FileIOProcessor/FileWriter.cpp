#include "async/FileIOProcessor.hpp"
#include "async/JobStatus.hpp"

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

FileWriter::FileWriter(unsigned int timeout_ms, int fd,
					   const std::string &content)
	: FileIOProcessor(timeout_ms, fd)
{
	_writer.setWriteBuf(content);
}

FileWriter::FileWriter(unsigned int timeout_ms, const std::string &path,
					   const std::string &content)
	: FileIOProcessor(timeout_ms, path)
{
	_writer.setWriteBuf(content);
}

FileWriter::~FileWriter()
{
}

int FileWriter::task(void)
{
	if (_status == JobStatus::OK)
		return (_status);

	checkTimeout();
	_writer.task();
	if (_writer.writeDone())
		_status = JobStatus::OK;
	return (_status);
}
