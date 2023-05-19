#include "async/SingleIOProcessor.hpp"
#include <errno.h>
#include <fcntl.h>
#include <sstream>
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
{
	operator=(orig);
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

void SingleIOProcessor::initialize()
{
	int result = fcntl(_fd, F_SETFL, O_NONBLOCK);
	if (result < 0)
		throw(std::runtime_error(strerror(errno)));
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
