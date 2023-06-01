#ifndef ASYNC_STATUS_HPP
#define ASYNC_STATUS_HPP

#include <string>

namespace async
{
namespace status
{
enum job_status_e
{
	OK_DONE = 0,
	OK_BEGIN,
	OK_AGAIN,
	ERROR_GENERIC,
	ERROR_FILECLOSED,
	ERROR_FILEOPENING,
	ERROR_FILEISDIR,
	ERROR_TIMEOUT,
	ERROR_READ,
	ERROR_WRITE
};
} // namespace status

std::string generateErrorMsgGeneric(void);
std::string generateErrorMsgFileClosed(const int fd);
std::string generateErrorMsgFileOpening(const std::string &path);
std::string generateErrorMsgFileIsDir(const std::string &path);
std::string generateErrorMsgTimeout(const int fd, const clock_t timeout);
std::string generateErrorMsgRead(const int fd);
std::string generateErrorMsgWrite(const int fd);
} // namespace async

#endif
