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

void AsyncSingleIOProcessor::getReadBuf(std::string &str)
{
	str += _rdbuf[_fd];
	_rdbuf[_fd] = "";
}

AsyncSingleIOProcessor &operator>>(AsyncSingleIOProcessor &io, std::string &str)
{
	io.getReadBuf(str);
	return (io);
}

namespace async
{
AsyncSingleIOProcessor cin(STDIN_FILENO);
AsyncSingleIOProcessor cout(STDOUT_FILENO);
AsyncSingleIOProcessor cerr(STDERR_FILENO);
} // namespace async
