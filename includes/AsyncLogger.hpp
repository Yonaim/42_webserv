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
	static int _log_filter_level;
	static int _log_level_default;
	const _Procs _target;
	const std::string _name;
	bool _should_write_prefix;
	int _log_level;

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
		bool newline;
		EndMarker(bool new_line = false);
		EndMarker(const EndMarker &orig);
		EndMarker &operator=(const EndMarker &orig);
		~EndMarker();
	};

	~AsyncLogger();
	void log(const std::string &content);
	void setEndMark(void);
	static void registerFd(int fd);
	static void setLogFilter(int log_filter);
	static void setDefaultLogLevel(int log_level);
	void setLogLevel(int log_level);
	static AsyncLogger &getLogger(const std::string &name);
	static void task(void);
	static void blockingWrite(void);
};

template <typename T> inline AsyncLogger &operator<<(AsyncLogger &io, T content)
{
	std::stringstream buf;
	buf << content;
	io.log(buf.str());
	return (io);
}

AsyncLogger &operator<<(AsyncLogger &io, const AsyncLogger::EndMarker mark);

namespace async
{
extern const AsyncLogger::EndMarker endl;
};

#endif
