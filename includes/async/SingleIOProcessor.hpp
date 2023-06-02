#ifndef ASYNC_SINGLEIOPROCESSOR_HPP
#define ASYNC_SINGLEIOPROCESSOR_HPP

#include "async/IOProcessor.hpp"
#include <sstream>

namespace async
{
class SingleIOProcessor : public IOProcessor
{
  private:
	int _fd;
	int _event_option;

	SingleIOProcessor();
	void initialize(void);
	virtual void task(void);

  public:
	enum IO_event_option
	{
		IO_R,
		IO_W,
		IO_RW
	};

	SingleIOProcessor(const int fd);
	SingleIOProcessor(const int fd, const int event_option);
	virtual ~SingleIOProcessor();

	void setWriteBuf(const std::string &str);
	void getReadBuf(std::string &str);
	bool writeDone(void);
	int getFd(void);
};

extern SingleIOProcessor cin;
extern SingleIOProcessor cout;
extern SingleIOProcessor cerr;
} // namespace async

template <typename T>
inline async::SingleIOProcessor &operator<<(async::SingleIOProcessor &io,
											T content)
{
	std::stringstream buf;
	buf << content;
	io.setWriteBuf(buf.str());
	return (io);
}

async::SingleIOProcessor &operator>>(async::SingleIOProcessor &io,
									 std::string &str);

#endif
