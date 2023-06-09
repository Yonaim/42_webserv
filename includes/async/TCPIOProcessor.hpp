#ifndef ASYNC_TCPIOPROCESSOR_HPP
#define ASYNC_TCPIOPROCESSOR_HPP

#include "async/IOProcessor.hpp"
#include "async/Logger.hpp"
#include <queue>

namespace async
{
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

	void finalize(const char *with_error);
	std::string &rdbuf(const int fd);
	std::string &wrbuf(const int fd);

	typedef fdIterator iterator;
	iterator begin(void);
	iterator end(void);
};
} // namespace async

#endif
