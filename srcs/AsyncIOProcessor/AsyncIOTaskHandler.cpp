#include "AsyncIOTaskHandler.hpp"
#include <algorithm>

AsyncIOTaskHandler::AsyncIOTaskHandler(void)
{
}

AsyncIOTaskHandler::~AsyncIOTaskHandler()
{
}

AsyncIOTaskHandler::AsyncIOTaskHandler(const AsyncIOTaskHandler &orig)
{
	(void)orig;
}

AsyncIOTaskHandler &AsyncIOTaskHandler::operator=(
	const AsyncIOTaskHandler &orig)
{
	(void)orig;
	return (*this);
}

AsyncIOTaskHandler &AsyncIOTaskHandler::getInstance(void)
{
	static AsyncIOTaskHandler instance;
	return (instance);
}

void AsyncIOTaskHandler::registerTask(AsyncIOProcessor *task)
{
	AsyncIOTaskHandler &instance = getInstance();
	_iterator it = std::lower_bound(instance._tasks.begin(),
									instance._tasks.end(), task);
	if (it != instance._tasks.end() && *it == task)
		return;
	instance._tasks.insert(it, task);
}

void AsyncIOTaskHandler::unregisterTask(AsyncIOProcessor *task)
{
	AsyncIOTaskHandler &instance = getInstance();
	_iterator it = std::lower_bound(instance._tasks.begin(),
									instance._tasks.end(), task);
	if (it == instance._tasks.end())
		return;
	instance._tasks.erase(it);
}

void AsyncIOTaskHandler::task(void)
{
	AsyncIOTaskHandler &instance = getInstance();
	for (_iterator it = instance._tasks.begin(); it != instance._tasks.end();
		 it++)
		(*it)->task();
}

void AsyncIOTaskHandler::blockingWrite(void)
{
	AsyncIOTaskHandler &instance = getInstance();
	for (_iterator it = instance._tasks.begin(); it != instance._tasks.end();
		 it++)
		(*it)->blockingWrite();
}
