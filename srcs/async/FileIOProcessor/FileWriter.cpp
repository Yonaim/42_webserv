#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"
#include <fcntl.h>

using namespace async;

FileWriter::FileWriter(unsigned int timeout_ms, int fd,
					   const std::string &content)
	: FileIOProcessor(timeout_ms, fd), _content(content)
{
}

FileWriter::FileWriter(unsigned int timeout_ms, const std::string &path,
					   const std::string &content)
	: FileIOProcessor(timeout_ms, path), _content(content)
{
}

FileWriter::~FileWriter()
{
}

int FileWriter::task(void)
{
	if (_status == status::OK_DONE)
		return (_status);
	if (_status == status::OK_BEGIN)
	{
		if (openFdByPath(O_WRONLY | O_CREAT))
			return (_status);
		_processor = new SingleIOProcessor(_fd, SingleIOProcessor::IO_W);
		_processor->setWriteBuf(_content);
		_status = status::OK_AGAIN;
	}

	if (checkTimeout())
		return (false);
	_processor->task();
	if (_processor->writeDone())
		_status = status::OK_DONE;
	return (_status);
}
