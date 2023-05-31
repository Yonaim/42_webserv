#include "async/IOProcessor.hpp"
#include "async/status.hpp"
#include "utils/string.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace async;

std::vector<IOProcessor *> IOProcessor::_objs;
const size_t IOProcessor::_buffsize = 2048;
bool IOProcessor::_debug = false;
static const timespec zerosec = {0, 0};

IOProcessor::IOProcessor(void) : _status(status::OK_BEGIN)
{
	initializeKQueue();
	registerObject(this);
}

IOProcessor::~IOProcessor()
{
	close(_kq);
	unregisterObject(this);
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

void IOProcessor::registerObject(IOProcessor *obj)
{
	std::vector<IOProcessor *>::iterator it
		= std::lower_bound(_objs.begin(), _objs.end(), obj);
	if (it != _objs.end() && *it == obj)
		return;
	_objs.insert(it, obj);
}

void IOProcessor::unregisterObject(IOProcessor *obj)
{
	std::vector<IOProcessor *>::iterator it
		= std::lower_bound(_objs.begin(), _objs.end(), obj);
	if (it == _objs.end())
		return;
	_objs.erase(it);
}

void IOProcessor::doAllTasks(void)
{
	for (std::vector<IOProcessor *>::iterator it = _objs.begin();
		 it != _objs.end(); it++)
		(*it)->task();
}

void IOProcessor::blockingWriteAll(void)
{
	for (std::vector<IOProcessor *>::iterator it = _objs.begin();
		 it != _objs.end(); it++)
		(*it)->blockingWrite();
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

int IOProcessor::read(const int fd, const size_t size)
{
	char *buff = new char[size];
	ssize_t readsize = ::read(fd, buff, size);
	if (readsize == 0)
	{
		delete[] buff;
		_status = status::ERROR_FILECLOSED;
		_error_msg = std::string("File ") + toStr(fd) + " is closed";
		return (_status);
	}
	if (readsize < 0)
	{
		delete[] buff;
		/*
		TODO: Checking the value of errno is strictly forbidden after a read or
		a write operation이지만 디버그 목적으로 strerror(errno) 사용하였고
		제출본에서는 삭제해야 함
		*/
		_status = status::ERROR_READ;
		_error_msg = std::string("Error while reading from file ") + toStr(fd);
		return (_status);
	}
	_rdbuf[fd].append(buff, readsize);
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Read " << readsize << " bytes: \""
				  << std::string(buff, buff + readsize) << "\"\n";
	delete[] buff;
	_status = status::OK_AGAIN;
	return (_status);
}

int IOProcessor::write(const int fd, const size_t size)
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
		_status = status::ERROR_WRITE;
		_error_msg = std::string("Error while writing to file ") + toStr(fd);
		return (_status);
	}
	if (_debug)
		std::cout << "IOProcessor:[DEBUG]:Wrote " << writesize << " bytes: \""
				  << getfrontstr(_wrbuf[fd], writesize) << "\"\n";
	trimfrontstr(_wrbuf[fd], writesize);
	_status = status::OK_AGAIN;
	return (_status);
}

const int &IOProcessor::stat(void) const
{
	return (_status);
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

/*
TODO: stat 함수가 사용 불가능하여 임시로 만든 해결책으로 stat이 사용 가능해질 시
해당 콜 사용하여 재작성해야 함
*/
bool IOProcessor::isFdClosed(const int fd)
{
	int rc = read(fd, 1);

	if (rc >= status::ERROR_GENERIC)
		return (true);
	return (false);
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
