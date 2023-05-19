#ifndef ASYNCLOGGER_HPP
#define ASYNCLOGGER_HPP

#include "AsyncSingleIOProcessor.hpp"
#include <map>
#include <sstream>
#include <string>
#include <vector>

class AsyncLogger
{
  private:
	typedef std::map<int, AsyncSingleIOProcessor *> _Procs;
	static std::map<std::string, AsyncLogger *> _loggers;
	static _Procs _target_default;
	static const std::string _name_default;
	static int _log_level;
	const _Procs _target;
	const std::string _name;
	std::string _buf;

	AsyncLogger(void);
	AsyncLogger(const std::string &name);
	AsyncLogger(const AsyncLogger &orig);
	AsyncLogger &operator=(const AsyncLogger &orig);

	std::string getPrefix(void);

  public:
	enum debug_level_e
	{
		DEBUG = 0,
		VERBOSE,
		INFO,
		WARNING,
		ERROR
	};
	class EndMarker
	{
	  public:
		int level;
		EndMarker(int _level = INFO);
		EndMarker(const EndMarker &orig);
		EndMarker &operator=(const EndMarker &orig);
		~EndMarker();
	};

	~AsyncLogger();
	void registerLog(const std::string &content);
	void log(int level);
	static void registerFd(int fd);
	static void setLogLevel(int log_level);
	static AsyncLogger &getLogger(const std::string &name);
	static void task(void);
	static void blockingWrite(void);
};

template <typename T> inline AsyncLogger &operator<<(AsyncLogger &io, T content)
{
	std::stringstream buf;
	buf << content;
	io.registerLog(buf.str());
	return (io);
}

AsyncLogger &operator<<(AsyncLogger &io, const AsyncLogger::EndMarker mark);

namespace async
{
extern const AsyncLogger::EndMarker debug;
extern const AsyncLogger::EndMarker verbose;
extern const AsyncLogger::EndMarker info;
extern const AsyncLogger::EndMarker warning;
extern const AsyncLogger::EndMarker error;
}; // namespace async

#endif
