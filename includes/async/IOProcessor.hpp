#ifndef ASYNC_IOPROCESSOR_HPP
#define ASYNC_IOPROCESSOR_HPP

#include "async/status.hpp"
#include <cstdlib>
#include <deque>
#include <map>
#include <string>
#include <sys/event.h>
#include <vector>

namespace async
{
class IOProcessor
{
  private:
	static std::vector<IOProcessor *> _objs;
	static const size_t _buffsize;
	int _kq;

	static void registerObject(IOProcessor *task);
	static void unregisterObject(IOProcessor *task);

  protected:
	int _status;
	int _event_count;
	std::string _error_msg;
	std::deque<struct kevent> _watchlist;
	std::deque<struct kevent> _eventlist;
	std::map<int, std::string> _rdbuf;
	std::map<int, std::string> _wrbuf;

	void initializeKQueue(void);
	void flushKQueue(void);
	int read(const int fd, const size_t size);
	int write(const int fd, const size_t size);
	virtual void task(void) = 0;

  public:
	IOProcessor(void);
	virtual ~IOProcessor();

	const int &stat(void) const;
	const std::string &errorMsg(void) const;
	static void doAllTasks(void);
	static void blockingWriteAll(void);
	void blockingWrite(void);
	int eventCount(void);
};

enum IOEVENT_E
{
	IOEVENT_READ = 0,
	IOEVENT_WRITE = 1,
	IOEVENT_ERROR = 2
};
} // namespace async

struct kevent constructKevent(const int fd, const int event);

#endif
