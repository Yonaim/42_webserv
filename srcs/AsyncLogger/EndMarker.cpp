#include "AsyncLogger.hpp"

const AsyncLogger::EndMarker async::endl(true);

AsyncLogger::EndMarker::EndMarker(bool new_line) : newline(new_line)
{
}

AsyncLogger::EndMarker::EndMarker(const AsyncLogger::EndMarker &orig)
	: newline(orig.newline)
{
}

AsyncLogger::EndMarker &AsyncLogger::EndMarker::operator=(
	const AsyncLogger::EndMarker &orig)
{
	newline = orig.newline;
	return (*this);
}

AsyncLogger::EndMarker::~EndMarker()
{
}
