#ifndef ASYNCIOTASKHANDLER_HPP
#define ASYNCIOTASKHANDLER_HPP

#include "AsyncIOProcessor.hpp"
#include <vector>

class AsyncIOTaskHandler
{
  private:
	typedef std::vector<AsyncIOProcessor *>::iterator _iterator;
	std::vector<AsyncIOProcessor *> _tasks;

	AsyncIOTaskHandler(void);
	~AsyncIOTaskHandler();
	AsyncIOTaskHandler(const AsyncIOTaskHandler &orig);
	AsyncIOTaskHandler &operator=(const AsyncIOTaskHandler &orig);
	static AsyncIOTaskHandler &getInstance(void);

  public:
	static void registerTask(AsyncIOProcessor *task);
	static void unregisterTask(AsyncIOProcessor *task);
	static void task(void);
};

#endif
