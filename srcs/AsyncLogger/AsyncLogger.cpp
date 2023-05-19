#include "AsyncLogger.hpp"
#include "AsyncIOTaskHandler.hpp"

std::map<std::string, AsyncLogger *> AsyncLogger::_loggers;
std::map<int, AsyncSingleIOProcessor *> AsyncLogger::_target_default;
const std::string AsyncLogger::_name_default = "root";
int AsyncLogger::_log_filter_level = INFO;
int AsyncLogger::_log_level_default = INFO;

AsyncLogger::AsyncLogger(void)
	: _target(_target_default), _name(_name_default),
	  _should_write_prefix(true), _log_level(_log_level_default)
{
}

AsyncLogger::AsyncLogger(const std::string &name)
	: _target(_target_default), _name(name), _should_write_prefix(true),
	  _log_level(_log_level_default)
{
}

AsyncLogger::AsyncLogger(const AsyncLogger &orig)
	: _target(orig._target), _name(orig._name),
	  _should_write_prefix(orig._should_write_prefix),
	  _log_level(orig._log_level)
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

void AsyncLogger::log(const std::string &content)
{
	if (_log_level < _log_filter_level)
		return;
	for (_Procs::const_iterator it = _target.begin(); it != _target.end(); it++)
	{
		if (_should_write_prefix)
			it->second->setWriteBuf(getPrefix());
		it->second->setWriteBuf(content);
	}
	_should_write_prefix = false;
}

void AsyncLogger::setEndMark(void)
{
	_should_write_prefix = true;
}

void AsyncLogger::setLogFilter(int log_filter)
{
	_log_filter_level = log_filter;
}

void AsyncLogger::setLogLevel(int log_level)
{
	_log_level_default = log_level;
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

AsyncLogger &operator<<(AsyncLogger &io, const AsyncLogger::EndMarker mark)
{
	if (mark.newline)
		io.log("\n");
	io.setEndMark();
	return (io);
}
