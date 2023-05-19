#include "async/Logger.hpp"
#include "async/IOTaskHandler.hpp"
#include <cstring>

using namespace async;

std::map<std::string, Logger *> Logger::_loggers;
std::map<int, SingleIOProcessor *> Logger::_target;
const std::string Logger::_name_default = "root";
int Logger::_log_level = INFO;
const char *Logger::_level_names[]
	= {"DEBUG  ", "VERBOSE", "INFO   ", "WARNING", "ERROR  "};

Logger::Logger(void) : _name(_name_default), _buf("")
{
}

Logger::Logger(const std::string &name) : _name(name), _buf("")
{
}

Logger::Logger(const Logger &orig) : _name(orig._name), _buf(orig._buf)
{
}

Logger &Logger::operator=(const Logger &orig)
{
	(void)orig;
	return (*this);
}

Logger::~Logger()
{
}

std::string Logger::getPrefix(int level)
{
	time_t now = time(NULL);
	struct tm *tstruct = localtime(&now);
	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tstruct);

	std::string prefix = std::string(buf) + ", " + "[" + _level_names[level]
						 + "] " + _name + ": ";

	return (prefix);
}

void Logger::registerLog(const std::string &content)
{
	_buf += content;
}

void Logger::log(int level)
{
	if (level < _log_level)
	{
		_buf = "";
		return;
	}
	for (_Procs::const_iterator it = _target.begin(); it != _target.end(); it++)
	{
		it->second->setWriteBuf(getPrefix(level));
		it->second->setWriteBuf(_buf);
	}
	_buf = "";
}

void Logger::setLogLevel(int log_filter)
{
	_log_level = log_filter;
}

void Logger::setLogLevel(const std::string &log_level)
{
	for (int i = 0; i < 5; i++)
	{
		if (::strncmp(log_level.c_str(), _level_names[i], log_level.size())
			== 0)
		{
			setLogLevel(i);
			return;
		}
	}
	throw(std::runtime_error("Log level " + log_level + " does not exist."));
}

void Logger::registerFd(int fd)
{
	if (_target.find(fd) == _target.end())
		_target[fd] = new SingleIOProcessor(fd);
}

Logger &Logger::getLogger(const std::string &name)
{
	if (_loggers.find(name) == _loggers.end())
	{
		Logger *new_logger = new Logger(name);
		_loggers[name] = new_logger;
	}
	return (*(_loggers[name]));
}

void Logger::task(void)
{
	IOTaskHandler::task();
}

void Logger::blockingWrite(void)
{
	IOTaskHandler::blockingWrite();
}

Logger &operator<<(Logger &io, const Logger::EndMarker mark)
{
	io.registerLog("\n");
	io.log(mark.level);
	return (io);
}
