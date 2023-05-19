#include "async/IOProcessor.hpp"

using namespace async;

IOProcessor::FileClosed::FileClosed(void) : std::runtime_error("File is closed")
{
}

IOProcessor::ReadError::ReadError(void)
	: std::runtime_error("Error while reading file")
{
}

IOProcessor::WriteError::WriteError(void)
	: std::runtime_error("Error while writing to file")
{
}
