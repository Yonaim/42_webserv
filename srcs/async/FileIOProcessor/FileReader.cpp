#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"
#include <fcntl.h>

using namespace async;

FileReader::FileReader(unsigned int timeout_ms, int fd, bool is_fifo)
	: FileIOProcessor(timeout_ms, fd), _is_fifo(is_fifo)
{
}

FileReader::FileReader(unsigned int timeout_ms, const std::string &path)
	: FileIOProcessor(timeout_ms, path), _is_fifo(false)
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
		openFdByPath(O_RDONLY);
		_processor = new SingleIOProcessor(_fd, SingleIOProcessor::IO_R);
		_status = status::OK_AGAIN;
		// TODO: stat 함수가 사용 가능해지면 isFdClosed는 삭제하든지 하고 파일의
		// 크기를 구하는 함수 작성하여 사용
		if (!_is_fifo && _processor->isFdClosed(_fd))
			_status = status::OK_DONE;
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
		_status = status::OK_DONE;
	}
	return (_status);
}
