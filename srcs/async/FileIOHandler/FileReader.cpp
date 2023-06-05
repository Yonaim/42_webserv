#include "async/FileIOHandler.hpp"
#include "async/status.hpp"
#include "utils/file.hpp"

using namespace async;

FileReader::FileReader(unsigned int timeout_ms, int fd, bool is_fifo)
	: FileIOHandler(timeout_ms, fd), _is_fifo(is_fifo)
{
}

FileReader::FileReader(unsigned int timeout_ms, const std::string &path)
	: FileIOHandler(timeout_ms, path), _is_fifo(false)
{
}

FileReader::~FileReader()
{
}

int FileReader::task(void)
{
	if (_status == status::OK_DONE)
		return (_status);
	if (_status == status::OK_BEGIN)
	{
		if (openFdByPath("r"))
		{
			return (_status);
		}
		if (!_is_fifo)
		{
			_filesize = getFileSize(_path);
			if (_filesize == 0)
			{
				_status = status::OK_DONE;
				return (_status);
			}
		}
		_processor = ft::shared_ptr<SingleIOProcessor>(
			new SingleIOProcessor(_fd, SingleIOProcessor::IO_R));
		_status = status::OK_AGAIN;
	}

	if (_processor->eventCount() > 0)
		renewTimeout();
	if (checkTimeout())
		return (_status);
	_processor->getReadBuf(_buffer);

	if (!_is_fifo && _filesize == _buffer.size())
	{
		_status = status::OK_DONE;
		return (_status);
	}

	switch (_processor->stat())
	{
	case status::ERROR_FILECLOSED:
		_processor->getReadBuf(_buffer);
		_status = status::OK_DONE;
		break;

	default:
		break;
	}
	return (_status);
}
