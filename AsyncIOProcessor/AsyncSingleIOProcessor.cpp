#include "AsyncSingleIOProcessor.hpp"
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

AsyncSingleIOProcessor::AsyncSingleIOProcessor()
{
}

AsyncSingleIOProcessor::AsyncSingleIOProcessor(const int fd) : _fd(fd)
{
	initialize();
}

AsyncSingleIOProcessor::~AsyncSingleIOProcessor()
{
}

AsyncSingleIOProcessor::AsyncSingleIOProcessor(
	const AsyncSingleIOProcessor &orig)
{
	operator=(orig);
}

AsyncSingleIOProcessor &AsyncSingleIOProcessor::operator=(
	const AsyncSingleIOProcessor &orig)
{
	_fd = orig._fd;
	initialize();
	return (*this);
}

void AsyncSingleIOProcessor::task(void)
{
	flushKQueue();
	while (!_eventlist.empty())
	{
		int flags = _eventlist.front().flags;
		int event = _eventlist.front().filter;
		_eventlist.pop_front();
		if (flags & EV_ERROR)
		{
			std::stringstream what;
			what << "I/O Error from fd " << _fd;
			throw(std::runtime_error(what.str()));
		}
		else if (event == EVFILT_READ)
			read(_fd);
		else if (event == EVFILT_WRITE && _wrbuf[_fd].length() > 0)
			write(_fd);
	}
}

void AsyncSingleIOProcessor::initialize()
{
	int result = fcntl(_fd, F_SETFL, O_NONBLOCK);
	if (result < 0)
		throw(std::runtime_error(strerror(errno)));
	_watchlist.push_back(constructKevent(_fd, IOEVENT_READ));
	_watchlist.push_back(constructKevent(_fd, IOEVENT_WRITE));
	flushKQueue();
}

void AsyncSingleIOProcessor::setWriteBuf(const std::string &str)
{
	_wrbuf[_fd] += str;
}

std::string AsyncSingleIOProcessor::getReadBuf(void)
{
	std::string buf = _rdbuf[_fd];
	_rdbuf[_fd] = "";
	return (buf);
}

AsyncSingleIOProcessor &operator<<(AsyncSingleIOProcessor &io,
								   const std::string &str)
{
	io.setWriteBuf(str);
	return (io);
}

AsyncSingleIOProcessor &operator>>(AsyncSingleIOProcessor &io, std::string &str)
{
	str = io.getReadBuf();
	return (io);
}

namespace async
{
AsyncSingleIOProcessor cout(STDOUT_FILENO);
AsyncSingleIOProcessor cin(STDIN_FILENO);
} // namespace async
