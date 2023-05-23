#ifndef ASYNC_IOTASKHANDLER_HPP
#define ASYNC_IOTASKHANDLER_HPP

#include "async/IOProcessor.hpp"
#include <vector>

namespace async
{
class IOTaskHandler
{
  private:
	typedef std::vector<IOProcessor *>::iterator _iterator;
	std::vector<IOProcessor *> _tasks;

	IOTaskHandler(void);
	virtual ~IOTaskHandler();
	IOTaskHandler(const IOTaskHandler &orig);
	IOTaskHandler &operator=(const IOTaskHandler &orig);
	static IOTaskHandler &getInstance(void);

  public:
	static void registerTask(IOProcessor *task);
	static void unregisterTask(IOProcessor *task);
	static void task(void);
	static void blockingWrite(void);
};
} // namespace async

#endif
