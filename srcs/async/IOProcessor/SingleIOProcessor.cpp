#include "async/SingleIOProcessor.hpp"
#include "utils/string.hpp"
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace async;

SingleIOProcessor::SingleIOProcessor()
{
}

SingleIOProcessor::SingleIOProcessor(const int fd) : _fd(fd)
{
	initialize();
}

SingleIOProcessor::~SingleIOProcessor()
{
}

SingleIOProcessor::SingleIOProcessor(const SingleIOProcessor &orig)
	: _fd(orig._fd)
{
	initialize();
}

SingleIOProcessor &SingleIOProcessor::operator=(const SingleIOProcessor &orig)
{
	_fd = orig._fd;
	initialize();
	return (*this);
}

void SingleIOProcessor::task(void)
{
	flushKQueue();
	while (!_eventlist.empty())
	{
		int flags = _eventlist.front().flags;
		int event = _eventlist.front().filter;
		int data = _eventlist.front().data;
		_eventlist.pop_front();
		if (flags & EV_ERROR)
		{
			throw(std::runtime_error(
				std::string("Failed to register EVFILT code " + toStr(event)
							+ " into kqueue, data " + toStr(data))));
		}
		else if (event == EVFILT_READ)
		{
			read(_fd, data);
		}
		else if (event == EVFILT_WRITE && _wrbuf[_fd].length() > 0)
		{
			size_t len = std::min((size_t)data, _wrbuf[_fd].length());
			/* 표준 출력같은 FIFO fd에서 발생하는
			Resource temporarily unavailable 오류 방지하는 임시 해결책 */
			if (len > 1)
				len /= 2;
			write(_fd, len);
		}
	}
}

void SingleIOProcessor::initialize()
{
	int result = fcntl(_fd, F_SETFL, O_NONBLOCK);
	if (result < 0)
		throw(std::runtime_error(std::string("Error while running fcntl at fd ")
								 + toStr(_fd) + ": " + strerror(errno)));
	_watchlist.push_back(constructKevent(_fd, IOEVENT_READ));
	_watchlist.push_back(constructKevent(_fd, IOEVENT_WRITE));
	flushKQueue();
}

void SingleIOProcessor::setWriteBuf(const std::string &str)
{
	_wrbuf[_fd] += str;
}

void SingleIOProcessor::getReadBuf(std::string &str)
{
	str += _rdbuf[_fd];
	_rdbuf[_fd] = "";
}

bool SingleIOProcessor::writeDone(void)
{
	return (_wrbuf[_fd].empty());
}

int SingleIOProcessor::getFd(void)
{
	return (_fd);
}

SingleIOProcessor &operator>>(SingleIOProcessor &io, std::string &str)
{
	io.getReadBuf(str);
	return (io);
}

namespace async
{
SingleIOProcessor cin(STDIN_FILENO);
SingleIOProcessor cout(STDOUT_FILENO);
SingleIOProcessor cerr(STDERR_FILENO);
} // namespace async
