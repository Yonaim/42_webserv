#ifndef ASYNC_STATUS_HPP
#define ASYNC_STATUS_HPP

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
} // namespace async

#endif
