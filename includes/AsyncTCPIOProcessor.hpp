#ifndef ASYNCTCPIOPROCESSOR_HPP
#define ASYNCTCPIOPROCESSOR_HPP

#include "AsyncIOProcessor.hpp"

class AsyncTCPIOProcessor : public AsyncIOProcessor
{
  private:
	typedef std::map<int, std::string>::iterator _iterator;
	static const int _backlog;
	int _port;
	int _listening_socket;

	void accept(void);
	void disconnect(const int client_socket);

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
	AsyncTCPIOProcessor(const int port = 80);
	virtual ~AsyncTCPIOProcessor();
	AsyncTCPIOProcessor(const AsyncTCPIOProcessor &orig);
	AsyncTCPIOProcessor &operator=(const AsyncTCPIOProcessor &orig);

	void initialize(void);
	void finalize(const char *with_error);
	virtual void task(void);
	std::string &rdbuf(const int fd);
	std::string &wrbuf(const int fd);

	typedef fdIterator iterator;
	iterator begin(void);
	iterator end(void);
};

#endif
