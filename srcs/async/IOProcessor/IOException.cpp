#include "async/IOProcessor.hpp"
#include "utils/string.hpp"

using namespace async;

IOProcessor::FileClosed::FileClosed(void) : std::runtime_error("File is closed")
{
}

IOProcessor::ReadError::ReadError(void)
	: std::runtime_error("Error while reading file")
{
}

IOProcessor::ReadError::ReadError(int fd, const std::string &why)
	: std::runtime_error("Error while reading fd " + toStr(fd) + ": " + why)
{
}

IOProcessor::WriteError::WriteError(void)
	: std::runtime_error("Error while writing to file")
{
}

IOProcessor::WriteError::WriteError(int fd, const std::string &why)
	: std::runtime_error("Error while writing to fd " + toStr(fd) + ": " + why)
{
}
