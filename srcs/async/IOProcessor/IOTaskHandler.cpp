#include "async/IOTaskHandler.hpp"
#include <algorithm>

using namespace async;

IOTaskHandler::IOTaskHandler(void)
{
}

IOTaskHandler::~IOTaskHandler()
{
}

IOTaskHandler::IOTaskHandler(const IOTaskHandler &orig)
{
	(void)orig;
}

IOTaskHandler &IOTaskHandler::operator=(const IOTaskHandler &orig)
{
	(void)orig;
	return (*this);
}

IOTaskHandler &IOTaskHandler::getInstance(void)
{
	static IOTaskHandler instance;
	return (instance);
}

void IOTaskHandler::registerTask(IOProcessor *task)
{
	IOTaskHandler &instance = getInstance();
	_iterator it = std::lower_bound(instance._tasks.begin(),
									instance._tasks.end(), task);
	if (it != instance._tasks.end() && *it == task)
		return;
	instance._tasks.insert(it, task);
}

void IOTaskHandler::unregisterTask(IOProcessor *task)
{
	IOTaskHandler &instance = getInstance();
	_iterator it = std::lower_bound(instance._tasks.begin(),
									instance._tasks.end(), task);
	if (it == instance._tasks.end())
		return;
	instance._tasks.erase(it);
}

void IOTaskHandler::task(void)
{
	IOTaskHandler &instance = getInstance();
	for (_iterator it = instance._tasks.begin(); it != instance._tasks.end();
		 it++)
	{
		// TODO: 궁극적으로 Error Status 기반 방식으로 선회
		try
		{
			(*it)->task();
		}
		catch (const std::exception &e)
		{
			(void)e;
		}
	}
}

void IOTaskHandler::blockingWrite(void)
{
	IOTaskHandler &instance = getInstance();
	for (_iterator it = instance._tasks.begin(); it != instance._tasks.end();
		 it++)
		(*it)->blockingWrite();
}
