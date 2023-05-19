#include "AsyncIOProcessor.hpp"

AsyncIOProcessor::FileClosed::FileClosed(void)
	: std::runtime_error("File is closed")
{
}

AsyncIOProcessor::ReadError::ReadError(void)
	: std::runtime_error("Error while reading file")
{
}

AsyncIOProcessor::WriteError::WriteError(void)
	: std::runtime_error("Error while writing to file")
{
}
