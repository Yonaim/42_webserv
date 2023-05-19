#ifndef ASYNCSINGLEIOPROCESSOR_HPP
#define ASYNCSINGLEIOPROCESSOR_HPP

#include "AsyncIOProcessor.hpp"
#include <sstream>

class AsyncSingleIOProcessor : public AsyncIOProcessor
{
  private:
	int _fd;

	AsyncSingleIOProcessor();
	void initialize(void);

  public:
	AsyncSingleIOProcessor(const int fd);
	virtual ~AsyncSingleIOProcessor();
	AsyncSingleIOProcessor(const AsyncSingleIOProcessor &orig);
	AsyncSingleIOProcessor &operator=(const AsyncSingleIOProcessor &orig);
	virtual void task(void);
	void setWriteBuf(const std::string &str);
	void getReadBuf(std::string &str);
	bool writeDone(void);
};

template <typename T>
inline AsyncSingleIOProcessor &operator<<(AsyncSingleIOProcessor &io, T content)
{
	std::stringstream buf;
	buf << content;
	io.setWriteBuf(buf.str());
	return (io);
}

AsyncSingleIOProcessor &operator>>(AsyncSingleIOProcessor &io,
								   std::string &str);

namespace async
{
extern AsyncSingleIOProcessor cin;
extern AsyncSingleIOProcessor cout;
extern AsyncSingleIOProcessor cerr;
} // namespace async

#endif
