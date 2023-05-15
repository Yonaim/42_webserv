#ifndef ASYNCIOTASKHANDLER_HPP
#define ASYNCIOTASKHANDLER_HPP

#include "AsyncIOProcessor.hpp"
#include <vector>

class AsyncIOTaskHandler
{
  private:
	typedef std::vector<AsyncIOProcessor *>::iterator _iterator;
	static std::vector<AsyncIOProcessor *> _tasks;

	AsyncIOTaskHandler(void);
	~AsyncIOTaskHandler();
	AsyncIOTaskHandler(const AsyncIOTaskHandler &orig);
	AsyncIOTaskHandler &operator=(const AsyncIOTaskHandler &orig);

  public:
	static void registerTask(AsyncIOProcessor *task);
	static void unregisterTask(AsyncIOProcessor *task);
	static void task(void);
};

#endif
