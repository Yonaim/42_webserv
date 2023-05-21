#ifndef IOPROCESSOR_HPP
#define IOPROCESSOR_HPP

#include <cstdlib>
#include <deque>
#include <map>
#include <string>
#include <sys/event.h>

namespace async
{
class IOProcessor
{
  private:
	static const size_t _buffsize;
	int _kq;

  protected:
	std::deque<struct kevent> _watchlist;
	std::deque<struct kevent> _eventlist;
	std::map<int, std::string> _rdbuf;
	std::map<int, std::string> _wrbuf;
	static bool _debug;

	void initializeKQueue(void);
	void flushKQueue(void);
	void read(const int fd, const size_t size);
	void write(const int fd, const size_t size);

  public:
	class FileClosed;
	class ReadError;
	class WriteError;

	IOProcessor(void);
	virtual ~IOProcessor();
	IOProcessor(const IOProcessor &orig);
	IOProcessor &operator=(const IOProcessor &orig);
	virtual void task(void) = 0;
	void blockingWrite(void);
	static void setDebug(bool debug);
};

enum IOEVENT_E
{
	IOEVENT_READ = 0,
	IOEVENT_WRITE = 1,
	IOEVENT_ERROR = 2
};

class IOProcessor::FileClosed : public std::runtime_error
{
  public:
	FileClosed(void);
};

class IOProcessor::ReadError : public std::runtime_error
{
  public:
	ReadError(void);
	ReadError(int fd, const std::string &why);
};

class IOProcessor::WriteError : public std::runtime_error
{
  public:
	WriteError(void);
	WriteError(int fd, const std::string &why);
};
} // namespace async

struct kevent constructKevent(const int fd, const int event);

#endif
