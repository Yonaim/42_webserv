#include "async/status.hpp"
#include "utils/string.hpp"

std::string async::generateErrorMsgGeneric(void)
{
	return (std::string("Unknown async error"));
}

std::string async::generateErrorMsgFileClosed(const int fd)
{
	return (std::string("File ") + toStr(fd) + " is closed");
}

std::string async::generateErrorMsgFileOpening(const std::string &path)
{
	return (std::string("Error opening file ") + path);
}

std::string async::generateErrorMsgFileIsDir(const std::string &path)
{
	return (std::string("File ") + path + " is directory");
}

std::string async::generateErrorMsgTimeout(const int fd, const clock_t timeout)
{
	return (std::string("Timeout (") + toStr(timeout / (CLOCKS_PER_SEC / 1000))
			+ " ms) occured while reading from fd ")
		   + toStr(fd);
}

std::string async::generateErrorMsgRead(const int fd)
{
	return (std::string("Error while reading from fd ") + toStr(fd));
}

std::string async::generateErrorMsgWrite(const int fd)
{
	return (std::string("Error while writing to fd ") + toStr(fd));
}
