#include "async/IOProcessor.hpp"
#include "async/IOTaskHandler.hpp"
#include "utils/string.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace async;

const size_t IOProcessor::_buffsize = 2048;
bool IOProcessor::_debug = false;
static const timespec zerosec = {0, 0};

IOProcessor::IOProcessor(void)
{
	initializeKQueue();
	IOTaskHandler::registerTask(this);
}

IOProcessor::~IOProcessor()
{
	close(_kq);
	IOTaskHandler::unregisterTask(this);
}

IOProcessor::IOProcessor(const IOProcessor &orig)
	: _watchlist(orig._watchlist), _eventlist(orig._eventlist),
	  _rdbuf(orig._rdbuf), _wrbuf(orig._wrbuf)
{
	initializeKQueue();
	flushKQueue();
}

IOProcessor &IOProcessor::operator=(const IOProcessor &orig)
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

void IOProcessor::initializeKQueue(void)
{
	_kq = kqueue();
	if (_kq < 0)
		throw(std::runtime_error("Failed to initialize kqueue."));
}

void IOProcessor::flushKQueue(void)
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
		throw(std::runtime_error(std::string("Error while running kevent: ")
								 + strerror(errno)));
	_eventlist.insert(_eventlist.end(), events, events + n_newevents);
}

void IOProcessor::read(const int fd, const size_t size)
{
	char *buff = new char[size];
	ssize_t readsize = ::read(fd, buff, size);
	if (readsize == 0)
	{
		delete[] buff;
		throw(IOProcessor::FileClosed());
	}
	if (readsize < 0)
	{
		delete[] buff;
		/*
		TODO: Checking the value of errno is strictly forbidden after a read or
		a write operation이지만 디버그 목적으로 strerror(errno) 사용하였고
		제출본에서는 삭제해야 함
		*/
		throw(IOProcessor::ReadError(fd, strerror(errno)));
	}
	_rdbuf[fd] += std::string(buff, buff + readsize);
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Read " << readsize << " bytes: \""
				  << buff << "\"\n";
	delete[] buff;
}

void IOProcessor::write(const int fd, const size_t size)
{
	ssize_t writesize = ::write(fd, _wrbuf[fd].c_str(), size);
	if (writesize == 0)
		throw(std::logic_error("write(2) call cannot return 0."));
	if (writesize < 0)
	{
		/*
		 TODO: Checking the value of errno is strictly forbidden after a read or
		 a write operation이지만 디버그 목적으로 strerror(errno) 사용하였고
		제출본에서는 삭제해야 함
		 */
		throw(IOProcessor::WriteError(fd, strerror(errno)));
	}
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Wrote " << writesize << " bytes: \""
				  << getfrontstr(_wrbuf[fd], writesize) << "\"\n";
	trimfrontstr(_wrbuf[fd], writesize);
}

void IOProcessor::blockingWrite(void)
{
	for (std::map<int, std::string>::iterator it = _wrbuf.begin();
		 it != _wrbuf.end(); it++)
	{
		while (!it->second.empty())
			task();
	}
}

void IOProcessor::setDebug(bool debug)
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
