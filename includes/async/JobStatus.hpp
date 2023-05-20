#ifndef JOBSTATUS_HPP
#define JOBSTATUS_HPP

namespace async
{
class JobStatus
{
  private:
	JobStatus(void);
	JobStatus(const JobStatus &orig);
	virtual ~JobStatus() = 0;
	virtual JobStatus &operator=(const JobStatus &orig) = 0;

  public:
	enum job_status_e
	{
		OK = 0,
		AGAIN,
		INVALID
	};
};
} // namespace async

#endif
