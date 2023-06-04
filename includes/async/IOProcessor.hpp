#ifndef ASYNC_IOPROCESSOR_HPP
#define ASYNC_IOPROCESSOR_HPP

#include "async/status.hpp"
#include <cstdlib>
#include <deque>
#include <map>
#include <string>
#include <sys/event.h>
#include <vector>

namespace async
{

/**
 * @class IOProcessor
 * @brief Asynchronous 입출력 처리를 담당하는 추상 클래스
 * 
 * 생성시, kqueue() 시스템 콜로 고유한 이벤트 큐를 생성하고 해당 식별자를
 * 멤버 변수 _kq에 저장한다. 또한, 생성된 객체를 정적 벡터 _objs에 저장하여,
 * 생성된 모든 IOProcessor의 I/O 작업을 균등하게 처리할 수 있도록 정적 함수
 * doAllTasks()를 제공한다.
 * IOProcessor의 주요 기능은 아래와 같다.
 *   1. 관찰할 이벤트를 등록하고 pending 상태인 이벤트를 감지한다.
 *   2. 파일디스크립터에 따라 입출력 버퍼를 관리한다.
 *   3. 비동기적으로 입출력 작업을 처리한다.
 * */

class IOProcessor
{
  private:
	static std::vector<IOProcessor *> _objs;
	static const size_t _buffsize;
	int _kq;

	static void registerObject(IOProcessor *task);
	static void unregisterObject(IOProcessor *task);

  protected:
	int _status;
	int _event_count;
	std::string _error_msg;
	std::deque<struct kevent> _watchlist;
	std::deque<struct kevent> _eventlist;
	std::map<int, std::string> _rdbuf;
	std::map<int, std::string> _wrbuf;

	void initializeKQueue(void);
	void flushKQueue(void);
	int read(const int fd, const size_t size);
	int write(const int fd, const size_t size);
	virtual void task(void) = 0;

  public:
	IOProcessor(void);
	virtual ~IOProcessor();
	IOProcessor(const IOProcessor &orig);
	IOProcessor &operator=(const IOProcessor &orig);

	const int &stat(void) const;
	const std::string &errorMsg(void) const;
	static void doAllTasks(void);
	static void blockingWriteAll(void);
	void blockingWrite(void);
	bool isFdClosed(const int fd);
	int eventCount(void);
};

enum IOEVENT_E
{
	IOEVENT_READ = 0,
	IOEVENT_WRITE = 1,
	IOEVENT_ERROR = 2
};
} // namespace async

struct kevent constructKevent(const int fd, const int event);

#endif
