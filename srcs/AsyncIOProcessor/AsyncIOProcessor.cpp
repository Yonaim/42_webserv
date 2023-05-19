#include "AsyncIOProcessor.hpp"
#include "AsyncIOTaskHandler.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

const size_t AsyncIOProcessor::_buffsize = 2048;
bool AsyncIOProcessor::_debug = false;
static const timespec zerosec = {0, 0};

AsyncIOProcessor::AsyncIOProcessor(void)
{
	initializeKQueue();
	AsyncIOTaskHandler::registerTask(this);
}

AsyncIOProcessor::~AsyncIOProcessor()
{
	close(_kq);
	AsyncIOTaskHandler::unregisterTask(this);
}

AsyncIOProcessor::AsyncIOProcessor(const AsyncIOProcessor &orig)
{
	initializeKQueue();
	operator=(orig);
}

AsyncIOProcessor &AsyncIOProcessor::operator=(const AsyncIOProcessor &orig)
{
	if (this == &orig)
		return (*this);
	close(_kq);
	_watchlist = orig._watchlist;
	_eventlist = orig._eventlist;
	_rdbuf = orig._rdbuf;
	_wrbuf = orig._wrbuf;
	initializeKQueue();
	flushKQueue();
	return (*this);
}

void AsyncIOProcessor::initializeKQueue(void)
{
	_kq = kqueue();
	if (_kq < 0)
		throw(std::runtime_error("Failed to initialize kqueue."));
}

void AsyncIOProcessor::flushKQueue(void)
{
	static const int size_eventbuf = 8;
	struct kevent events[size_eventbuf];
	int n_newevents;
	if (_watchlist.empty())
		n_newevents = kevent(_kq, NULL, 0, events, size_eventbuf, &zerosec);
	else
		n_newevents = kevent(_kq, &_watchlist[0], _watchlist.size(), events,
							 size_eventbuf, &zerosec);
	_watchlist.clear();
	if (n_newevents < 0)
		throw(std::runtime_error(strerror(errno)));
	_eventlist.insert(_eventlist.end(), events, events + n_newevents);
}

void AsyncIOProcessor::read(const int fd)
{
	char buff[_buffsize + 1];
	ssize_t readsize = ::read(fd, buff, _buffsize);
	if (readsize <= 0)
	{
		std::stringstream what;
		if (readsize < 0)
			what << "Error while reading from fd " << fd << ": "
				 << strerror(errno);
		else
			what << "fd " << fd << " closed.";
		throw(std::runtime_error(what.str()));
		return;
	}
	buff[readsize] = '\0';
	_rdbuf[fd] += buff;
	if (_debug)
		std::cout << "AsyncIOProcessor:[DEBUG]:Read " << readsize
				  << " bytes: \"" << buff << "\"\n";
}

void AsyncIOProcessor::write(const int fd)
{
	ssize_t writesize = ::write(fd, _wrbuf[fd].c_str(), _wrbuf[fd].length());
	if (writesize <= 0)
	{
		std::stringstream what;
		if (writesize == 0)
			throw(std::logic_error("write(2) call cannot return 0."));
		what << "Error while writing to fd " << fd << ": " << strerror(errno);
		throw(std::runtime_error(what.str()));
		return;
	}
	if (_debug)
		std::cout << "AsyncIOProcessor:[DEBUG]:Wrote " << writesize
				  << " bytes: \"" << _wrbuf[fd].substr(0, writesize) << "\"\n";
	_wrbuf[fd] = _wrbuf[fd].substr(writesize, _wrbuf[fd].length());
}

void AsyncIOProcessor::blockingWrite(void)
{
	for (std::map<int, std::string>::iterator it = _wrbuf.begin();
		 it != _wrbuf.end(); it++)
	{
		while (!it->second.empty())
			write(it->first);
	}
}

void AsyncIOProcessor::setDebug(bool debug)
{
	_debug = debug;
}

struct kevent constructKevent(const int fd, const int event)
{
	if (event == IOEVENT_ERROR)
		throw(std::logic_error("ERROR state cannot be converted into kevent."));
	struct kevent output;
	int filter;
	switch (event)
	{
	case IOEVENT_READ:
		filter = EVFILT_READ;
		break;
	case IOEVENT_WRITE:
		filter = EVFILT_WRITE;
		break;
	default:
		throw(std::logic_error("Not implemented event."));
	}
	EV_SET(&output, fd, filter, EV_ADD | EV_ENABLE, 0, 0, NULL);
	return (output);
}
