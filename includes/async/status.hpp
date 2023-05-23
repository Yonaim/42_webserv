#ifndef ASYNC_STATUS_HPP
#define ASYNC_STATUS_HPP

namespace async
{
namespace status
{
enum job_status_e
{
	OK = 0,
	BEGIN,
	AGAIN,
	INVALID
};
} // namespace status
} // namespace async

#endif
