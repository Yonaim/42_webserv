#include "AsyncLogger.hpp"

const AsyncLogger::EndMarker async::debug(AsyncLogger::DEBUG);
const AsyncLogger::EndMarker async::verbose(AsyncLogger::VERBOSE);
const AsyncLogger::EndMarker async::info(AsyncLogger::INFO);
const AsyncLogger::EndMarker async::warning(AsyncLogger::WARNING);
const AsyncLogger::EndMarker async::error(AsyncLogger::ERROR);

AsyncLogger::EndMarker::EndMarker(int _level) : level(_level)
{
}

AsyncLogger::EndMarker::EndMarker(const AsyncLogger::EndMarker &orig)
	: level(orig.level)
{
}

AsyncLogger::EndMarker &AsyncLogger::EndMarker::operator=(
	const AsyncLogger::EndMarker &orig)
{
	level = orig.level;
	return (*this);
}

AsyncLogger::EndMarker::~EndMarker()
{
}
