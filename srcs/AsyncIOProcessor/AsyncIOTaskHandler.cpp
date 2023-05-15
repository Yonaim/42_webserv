#include "AsyncIOTaskHandler.hpp"
#include <algorithm>

std::vector<AsyncIOProcessor *> AsyncIOTaskHandler::_tasks;

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

void AsyncIOTaskHandler::registerTask(AsyncIOProcessor *task)
{
	_iterator it = std::lower_bound(_tasks.begin(), _tasks.end(), task);
	if (it != _tasks.end() && *it == task)
		return;
	_tasks.insert(it, task);
}

void AsyncIOTaskHandler::unregisterTask(AsyncIOProcessor *task)
{
	_iterator it = std::lower_bound(_tasks.begin(), _tasks.end(), task);
	if (it == _tasks.end())
		return;
	_tasks.erase(it);
}

void AsyncIOTaskHandler::task(void)
{
	for (_iterator it = _tasks.begin(); it != _tasks.end(); it++)
		(*it)->task();
}
