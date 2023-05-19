#include "AsyncLogger.hpp"
#include "AsyncIOTaskHandler.hpp"
#include <cstring>

std::map<std::string, AsyncLogger *> AsyncLogger::_loggers;
std::map<int, AsyncSingleIOProcessor *> AsyncLogger::_target;
const std::string AsyncLogger::_name_default = "root";
int AsyncLogger::_log_level = INFO;
const char *AsyncLogger::_level_names[]
	= {"DEBUG  ", "VERBOSE", "INFO   ", "WARNING", "ERROR  "};

AsyncLogger::AsyncLogger(void) : _name(_name_default), _buf("")
{
}

AsyncLogger::AsyncLogger(const std::string &name) : _name(name), _buf("")
{
}

AsyncLogger::AsyncLogger(const AsyncLogger &orig)
	: _name(orig._name), _buf(orig._buf)
{
}

AsyncLogger &AsyncLogger::operator=(const AsyncLogger &orig)
{
	(void)orig;
	return (*this);
}

AsyncLogger::~AsyncLogger()
{
}

std::string AsyncLogger::getPrefix(int level)
{
	time_t now = time(NULL);
	struct tm *tstruct = localtime(&now);
	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tstruct);

	std::string prefix = std::string(buf) + ", " + "[" + _level_names[level]
						 + "] " + _name + ": ";

	return (prefix);
}

void AsyncLogger::registerLog(const std::string &content)
{
	_buf += content;
}

void AsyncLogger::log(int level)
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

void AsyncLogger::setLogLevel(int log_filter)
{
	_log_level = log_filter;
}

void AsyncLogger::setLogLevel(const std::string &log_level)
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

void AsyncLogger::registerFd(int fd)
{
	if (_target.find(fd) == _target.end())
		_target[fd] = new AsyncSingleIOProcessor(fd);
}

AsyncLogger &AsyncLogger::getLogger(const std::string &name)
{
	if (_loggers.find(name) == _loggers.end())
	{
		AsyncLogger *new_logger = new AsyncLogger(name);
		_loggers[name] = new_logger;
	}
	return (*(_loggers[name]));
}

void AsyncLogger::task(void)
{
	AsyncIOTaskHandler::task();
}

void AsyncLogger::blockingWrite(void)
{
	AsyncIOTaskHandler::blockingWrite();
}

AsyncLogger &operator<<(AsyncLogger &io, const AsyncLogger::EndMarker mark)
{
	io.registerLog("\n");
	io.log(mark.level);
	return (io);
}
