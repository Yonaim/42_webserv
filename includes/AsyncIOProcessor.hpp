#ifndef ASYNCIOPROCESSOR_HPP
#define ASYNCIOPROCESSOR_HPP

#include <cstdlib>
#include <deque>
#include <map>
#include <sys/event.h>

class AsyncIOProcessor
{
  private:
	static const size_t _buffsize;
	int _kq;

  protected:
	std::deque<struct kevent> _watchlist;
	std::deque<struct kevent> _eventlist;
	std::map<int, std::string> _rdbuf;
	std::map<int, std::string> _wrbuf;
	void initializeKQueue(void);
	void flushKQueue(void);
	void read(const int fd);
	void write(const int fd);

  public:
	AsyncIOProcessor(void);
	virtual ~AsyncIOProcessor();
	AsyncIOProcessor(const AsyncIOProcessor &orig);
	AsyncIOProcessor &operator=(const AsyncIOProcessor &orig);
	virtual void task(void) = 0;
};

enum IOEVENT_E
{
	IOEVENT_READ = 0,
	IOEVENT_WRITE = 1,
	IOEVENT_ERROR = 2
};

struct kevent constructKevent(const int fd, const int event);

#endif
