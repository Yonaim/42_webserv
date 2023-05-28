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

SingleIOProcessor::SingleIOProcessor(const int fd)
	: _fd(fd), _event_option(IO_RW)
{
	initialize();
}

SingleIOProcessor::SingleIOProcessor(const int fd, const int event_option)
	: _fd(fd), _event_option(event_option)
{
	initialize();
}

SingleIOProcessor::~SingleIOProcessor()
{
}

SingleIOProcessor::SingleIOProcessor(const SingleIOProcessor &orig)
	: _fd(orig._fd), _event_option(orig._event_option)
{
	initialize();
}

SingleIOProcessor &SingleIOProcessor::operator=(const SingleIOProcessor &orig)
{
	_fd = orig._fd;
	_event_option = orig._event_option;
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
			/* 표준 출력같은 FIFO fd에서 발생하는 Resource temporarily
			 * unavailable 오류 무시 */
			try
			{
				write(_fd, _wrbuf[_fd].length());
			}
			catch (const IOProcessor::WriteError &e)
			{
				(void)e;
			}
		}
	}
}

void SingleIOProcessor::initialize()
{
	int result = fcntl(_fd, F_SETFL, O_NONBLOCK);
	if (result < 0)
		throw(std::runtime_error(std::string("Error while running fcntl at fd ")
								 + toStr(_fd) + ": " + strerror(errno)));
	if (_event_option == IO_R)
		_watchlist.push_back(constructKevent(_fd, IOEVENT_READ));
	else if (_event_option == IO_W)
		_watchlist.push_back(constructKevent(_fd, IOEVENT_WRITE));
	else
	{
		_watchlist.push_back(constructKevent(_fd, IOEVENT_READ));
		_watchlist.push_back(constructKevent(_fd, IOEVENT_WRITE));
	}
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
