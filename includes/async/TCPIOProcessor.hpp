#ifndef ASYNC_TCPIOPROCESSOR_HPP
#define ASYNC_TCPIOPROCESSOR_HPP

#include "async/IOProcessor.hpp"
#include "async/Logger.hpp"
#include <queue>

namespace async
{
/**
 * @class TCPIOProcessor
 * @brief TCP 서버가 클라이언트와의 연결을 관리하고 비동기적으로 입출력을
 * 처리하는 클래스
 *
 * TCPIOProcessor는 클라이언트 소켓 파일의 파일 디스크립터에 따라 서로 다른
 * 클라이언트를 관리한다. 주어진 포트로 들어온 클라이언트의 연결을
 * 승인(accept)하거나 클라이언트의 소켓 파일에 대한 읽기 및 쓰기 작업을 처리한다.
 */
class TCPIOProcessor : public IOProcessor
{
  private:
	typedef std::map<int, std::string>::iterator _iterator;
	int _port;
	int _backlog_size;
	int _listening_socket;
	Logger &_logger;

	void accept(void);
	void disconnect(const int client_socket);
	virtual void task(void);

	class fdIterator
	{
	  private:
		_iterator _it;

		fdIterator();

	  public:
		~fdIterator();
		fdIterator(const fdIterator &orig);
		fdIterator &operator=(const fdIterator &orig);
		fdIterator(const _iterator &it);
		fdIterator &operator++(void);
		const fdIterator operator++(int);
		bool operator!=(const fdIterator &other) const;
		int operator*(void);
	};

  public:
	static std::queue<int> disconnected_clients;

	TCPIOProcessor(const int port = 80, const int backlog = 8);
	virtual ~TCPIOProcessor();
	TCPIOProcessor(const TCPIOProcessor &orig);
	TCPIOProcessor &operator=(const TCPIOProcessor &orig);

	void initialize(void);
	void finalize(const char *with_error);
	std::string &rdbuf(const int fd);
	std::string &wrbuf(const int fd);

	typedef fdIterator iterator;
	iterator begin(void);
	iterator end(void);
};
} // namespace async

#endif
