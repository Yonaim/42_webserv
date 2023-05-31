#ifndef ASYNC_LOGGER_HPP
#define ASYNC_LOGGER_HPP

#include "async/SingleIOProcessor.hpp"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace async
{
class Logger
{
  private:
	typedef std::map<int, SingleIOProcessor *> _Procs;

	static std::map<std::string, Logger *> _loggers;
	static _Procs _target;
	static const std::string _name_default;
	static int _log_level;
	static const char *_level_names[];
	static bool _active;

	const std::string _name;

	Logger(void);
	Logger(const std::string &name);
	Logger(const Logger &orig);
	Logger &operator=(const Logger &orig);

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

	~Logger();
	std::string getPrefix(int level);
	void log(const std::string &content);
	static bool isActive(void);
	static void activate(void);
	static void deactivate(void);
	static void registerFd(int fd);
	static void setLogLevel(int log_level);
	static void setLogLevel(const std::string &log_level);
	static int getLogLevel(void);
	static Logger &getLogger(const std::string &name);
	static void doAllTasks(void);
	static void blockingWriteAll(void);
};

extern const Logger::EndMarker debug;
extern const Logger::EndMarker verbose;
extern const Logger::EndMarker info;
extern const Logger::EndMarker warning;
extern const Logger::EndMarker error;
} // namespace async

async::Logger &operator<<(async::Logger &io,
						  const async::Logger::EndMarker mark);

template <typename T>
inline async::Logger &operator<<(async::Logger &io, T content)
{
	if (!io.isActive())
		return (io);
	std::stringstream buf;
	buf << content;
	io.log(buf.str());
	return (io);
}

#define ASYNC_LOG_DEBUG(logger, message)                                       \
	do                                                                         \
	{                                                                          \
		if (async::Logger::DEBUG >= async::Logger::getLogLevel())              \
			logger << async::debug << message;                                 \
	} while (0)

#endif
