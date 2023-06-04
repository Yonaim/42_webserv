#ifndef ASYNC_LOGGER_HPP
#define ASYNC_LOGGER_HPP

#include "async/SingleIOProcessor.hpp"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace async
{
/**
 * @class Logger
 * @brief 로그 메시지를 생성하고 출력하는 클래스
 *
 * Logger는 로그 메시지를 생성하고 비동기적 및 논블로킹 방식으로 로그 메시지를
 * 출력하는 기능을 제공한다. 로그 메시지는 로그 레벨과 타임 스탬프를 포함한다.
 * 출력할 파일 디스크립터와 로깅 레벨을 설정하면, 로그 메시지의 로그 레벨에 따라
 * 설정한 파일 디스크립터에 메시지가 출력된다. Logger 객체는 getLogger() 정적
 * 함수를 사용하여 생성할 수 있으며, 모든 Logger 객체는 문자열을 키로 하고
 * Logger의 포인터를 값으로 하는 _loggers 맵에 저장하여 전체적인 로그 출력을
 * 관리한다.
 */
/**
 * @class Logger::EndMarker
 * @brief 로그 메시지의 레벨을 설정하는 Logger 내부에 정의되어있는 클래스
 *
 * EndMarker는 로그 메시지의 레벨을 설정한다. Logger는 프로그램의 로깅 레벨과
 * 로그 메시지의 로그 레벨을 비교하여 Logger의 버퍼에 해당 로그 메시지를
 * 저장할지를 결정한다.
 *
 */
class Logger
{
  private:
	typedef std::map<int, SingleIOProcessor *> _Procs;

	static std::map<std::string, Logger *> _loggers;
	static _Procs _target;
	static const std::string _name_default;
	static int _log_level;
	static const char *_level_names[];
	static const char *_level_prefixes[];
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

// _logger라는 이름의 로거 객체가 있을때 사용 가능
#define LOG_DEBUG(message)                                                     \
	do                                                                         \
	{                                                                          \
		if (async::Logger::DEBUG >= async::Logger::getLogLevel())              \
			_logger << async::debug << message;                                \
	} while (0)

#endif
