#include "async/IOProcessor.hpp"
#include "async/IOTaskHandler.hpp"
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
{
	initializeKQueue();
	operator=(orig);
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
		throw(std::runtime_error(strerror(errno)));
	_eventlist.insert(_eventlist.end(), events, events + n_newevents);
}

void IOProcessor::read(const int fd)
{
	char buff[_buffsize + 1];
	ssize_t readsize = ::read(fd, buff, _buffsize);
	if (readsize == 0)
		throw(IOProcessor::FileClosed());
	if (readsize < 0)
		throw(IOProcessor::ReadError());
	buff[readsize] = '\0';
	_rdbuf[fd] += buff;
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Read " << readsize << " bytes: \""
				  << buff << "\"\n";
}

void IOProcessor::write(const int fd)
{
	ssize_t writesize = ::write(fd, _wrbuf[fd].c_str(), _wrbuf[fd].length());
	if (writesize == 0)
		throw(std::logic_error("write(2) call cannot return 0."));
	if (writesize < 0)
		throw(IOProcessor::WriteError());
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Wrote " << writesize << " bytes: \""
				  << _wrbuf[fd].substr(0, writesize) << "\"\n";
	_wrbuf[fd] = _wrbuf[fd].substr(writesize, _wrbuf[fd].length());
}

void IOProcessor::blockingWrite(void)
{
	for (std::map<int, std::string>::iterator it = _wrbuf.begin();
		 it != _wrbuf.end(); it++)
	{
		while (!it->second.empty())
			write(it->first);
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
