#include "AsyncLogger.hpp"
#include "AsyncIOTaskHandler.hpp"

std::map<std::string, AsyncLogger *> AsyncLogger::_loggers;
std::map<int, AsyncSingleIOProcessor *> AsyncLogger::_target_default;
const std::string AsyncLogger::_name_default = "root";
int AsyncLogger::_log_level = INFO;

AsyncLogger::AsyncLogger(void)
	: _target(_target_default), _name(_name_default), _buf("")
{
}

AsyncLogger::AsyncLogger(const std::string &name)
	: _target(_target_default), _name(name), _buf("")
{
}

AsyncLogger::AsyncLogger(const AsyncLogger &orig)
	: _target(orig._target), _name(orig._name), _buf(orig._buf)
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

std::string AsyncLogger::getPrefix(void)
{
	// TODO: implementation
	return (_name + ": ");
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
		it->second->setWriteBuf(getPrefix());
		it->second->setWriteBuf(_buf);
	}
	_buf = "";
}

void AsyncLogger::setLogLevel(int log_filter)
{
	_log_level = log_filter;
}

void AsyncLogger::registerFd(int fd)
{
	if (_target_default.find(fd) == _target_default.end())
		_target_default[fd] = new AsyncSingleIOProcessor(fd);
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
	io.log(mark.level);
	return (io);
}
