#ifndef ASYNC_SINGLEIOPROCESSOR_HPP
#define ASYNC_SINGLEIOPROCESSOR_HPP

#include "async/IOProcessor.hpp"
#include <sstream>

namespace async
{
/**
 * @class SingleIOProcessor
 * @brief 단일 파일디스크립터의 Asynchronous Non-Blocking 입출력 처리를 담당하는
 *        IOProcessor의 유도 클래스
 * 
 * 주어진 파일 디스크립터를 비차단(non-blocking)으로 설정하고 멤버 변수 _fd에 저장한다.
 * 특정 파일 디스크립터(_fd)에 대한 Asynchronous Non-Blocking I/O를 처리한다.
 */
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
	SingleIOProcessor(const SingleIOProcessor &orig);
	SingleIOProcessor &operator=(const SingleIOProcessor &orig);

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
