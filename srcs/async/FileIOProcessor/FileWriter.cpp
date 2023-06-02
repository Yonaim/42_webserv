#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"

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
		if (openFdByPath("w"))
			return (_status);
		_processor = ft::shared_ptr<SingleIOProcessor>(
			new SingleIOProcessor(_fd, SingleIOProcessor::IO_W));
		_processor->setWriteBuf(_content);
		_status = status::OK_AGAIN;
	}

	if (_processor->eventCount() > 0)
		renewTimeout();
	if (checkTimeout())
		return (_status);
	if (_processor->writeDone())
		_status = status::OK_DONE;
	return (_status);
}
