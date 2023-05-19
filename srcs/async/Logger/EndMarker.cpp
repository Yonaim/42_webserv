#include "async/Logger.hpp"

using namespace async;

const Logger::EndMarker async::debug(Logger::DEBUG);
const Logger::EndMarker async::verbose(Logger::VERBOSE);
const Logger::EndMarker async::info(Logger::INFO);
const Logger::EndMarker async::warning(Logger::WARNING);
const Logger::EndMarker async::error(Logger::ERROR);

Logger::EndMarker::EndMarker(int _level) : level(_level)
{
}

Logger::EndMarker::EndMarker(const Logger::EndMarker &orig) : level(orig.level)
{
}

Logger::EndMarker &Logger::EndMarker::operator=(const Logger::EndMarker &orig)
{
	level = orig.level;
	return (*this);
}

Logger::EndMarker::~EndMarker()
{
}
