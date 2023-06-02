#include "async/Logger.hpp"
#include "utils/ansi_escape.h"
#include <cstring>

using namespace async;

std::map<std::string, ft::shared_ptr<Logger> > Logger::_loggers;
std::map<int, ft::shared_ptr<SingleIOProcessor> > Logger::_target;
const std::string Logger::_name_default = "root";
int Logger::_log_level = INFO;
const char *Logger::_level_names[]
	= {"DEBUG  ", "VERBOSE", "INFO   ", "WARNING", "ERROR  "};
const char *Logger::_level_prefixes[]
	= {ANSI_RESET, ANSI_RESET, ANSI_BWHITE, ANSI_BYELLOW, ANSI_BRED};
bool Logger::_active = false;

Logger::Logger(void) : _name(_name_default)
{
}

Logger::Logger(const std::string &name) : _name(name)
{
}

Logger::Logger(const Logger &orig) : _name(orig._name)
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

	std::stringstream prefix;
	prefix << _level_prefixes[level] << buf << ", [" << _level_names[level]
		   << "]" << _name << ": ";
	return (prefix.str());
}

void Logger::log(const std::string &content)
{
	if (!_active)
		return;
	for (_Procs::iterator it = _target.begin(); it != _target.end(); it++)
	{
		_SingleIOPtr &logger = it->second;
		logger->setWriteBuf(content);
	}
}

bool Logger::isActive(void)
{
	return (_active);
}

void Logger::activate(void)
{
	_active = true;
}

void Logger::deactivate(void)
{
	_active = false;
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

int Logger::getLogLevel(void)
{
	return (_log_level);
}

void Logger::registerFd(int fd)
{
	if (_target.find(fd) == _target.end())
		_target[fd] = _SingleIOPtr(new SingleIOProcessor(fd));
}

Logger &Logger::getLogger(const std::string &name)
{
	if (_loggers.find(name) == _loggers.end())
		_loggers[name] = ft::shared_ptr<Logger>(new Logger(name));
	return (*(_loggers[name]));
}

void Logger::doAllTasks(void)
{
	IOProcessor::doAllTasks();
}

void Logger::blockingWriteAll(void)
{
	IOProcessor::blockingWriteAll();
}

Logger &operator<<(Logger &io, const Logger::EndMarker mark)
{
	io.log(ANSI_RESET "\n");
	if (mark.level < Logger::getLogLevel())
		Logger::deactivate();
	else
		Logger::activate();
	io.log(io.getPrefix(mark.level));
	return (io);
}
