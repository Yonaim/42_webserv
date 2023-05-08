#include "AsyncTCPIOProcessor.hpp"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int AsyncTCPIOProcessor::_backlog = 8;

AsyncTCPIOProcessor::AsyncTCPIOProcessor(const int port) : _port(port)
{
	initialize();
}

AsyncTCPIOProcessor::~AsyncTCPIOProcessor()
{
	finalize(NULL);
}

AsyncTCPIOProcessor::AsyncTCPIOProcessor(const AsyncTCPIOProcessor &orig)
{
	operator=(orig);
}

AsyncTCPIOProcessor &AsyncTCPIOProcessor::operator=(
	const AsyncTCPIOProcessor &orig)
{
	finalize(NULL);
	_port = orig._port;
	initialize();
	return (*this);
}

void AsyncTCPIOProcessor::task(void)
{
	flushKQueue();
	while (!_eventlist.empty())
	{
		struct kevent event = _eventlist.front();
		_eventlist.pop_front();
		if (event.flags & EV_ERROR)
		{
			if (static_cast<int>(event.ident) == _listening_socket)
				finalize("Error from server socket");
			else
			{
				std::cerr << "Error from client socket" << std::endl;
				disconnect(event.ident);
			}
		}
		else if (event.filter == EVFILT_READ)
		{
			if (static_cast<int>(event.ident) == _listening_socket)
				accept();
			else
			{
				try
				{
					read(event.ident);
				}
				catch (const std::runtime_error &e)
				{
					std::cerr << e.what() << std::endl;
					disconnect(event.ident);
				}
			}
		}
		else if (event.filter == EVFILT_WRITE)
		{
			if (_wrbuf[event.ident].length() > 0)
			{
				try
				{
					write(event.ident);
				}
				catch (const std::runtime_error &e)
				{
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}
}

void AsyncTCPIOProcessor::initialize(void)
{
	int result;
	_listening_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_listening_socket < 0)
		throw(std::runtime_error(strerror(errno)));
	std::cout << "Created socket " << _listening_socket << std::endl;

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);
	result = bind(_listening_socket, (struct sockaddr *)&addr,
				  sizeof(struct sockaddr_in));
	if (result < 0)
		finalize(strerror(errno));
	std::cout << "Bind socket " << _listening_socket << " at port " << _port
			  << std::endl;

	result = listen(_listening_socket, _backlog);
	if (result < 0)
		finalize(strerror(errno));
	std::cout << "Listen with backlog size " << _backlog << std::endl;

	result = fcntl(_listening_socket, F_SETFL, O_NONBLOCK);
	if (result < 0)
		finalize(strerror(errno));
	_watchlist.push_back(constructKevent(_listening_socket, IOEVENT_READ));
	flushKQueue();
	std::cout << "AsyncTCPIOProcessor initialization complete" << std::endl;
}

void AsyncTCPIOProcessor::finalize(const char *with_error)
{
	close(_listening_socket);
	_listening_socket = 0;
	if (with_error)
		throw(std::runtime_error(with_error));
}

void AsyncTCPIOProcessor::accept(void)
{
	int new_client_socket = ::accept(_listening_socket, NULL, NULL);
	if (new_client_socket < 0)
		finalize(strerror(errno));
	std::cout << "Accepted new client: " << new_client_socket << std::endl;
	int result = fcntl(new_client_socket, F_SETFL, O_NONBLOCK);
	if (result < 0)
		finalize(strerror(errno));
	_watchlist.push_back(constructKevent(new_client_socket, IOEVENT_READ));
	_watchlist.push_back(constructKevent(new_client_socket, IOEVENT_WRITE));
	_rdbuf[new_client_socket] = "";
	_wrbuf[new_client_socket] = "";
}

void AsyncTCPIOProcessor::disconnect(const int client_socket)
{
	close(client_socket);
	_rdbuf.erase(client_socket);
	_wrbuf.erase(client_socket);
	std::cout << "Disconnected " << client_socket << std::endl;
}

std::string &AsyncTCPIOProcessor::rdbuf(const int fd)
{
	return (_rdbuf[fd]);
}

std::string &AsyncTCPIOProcessor::wrbuf(const int fd)
{
	return (_wrbuf[fd]);
}

AsyncTCPIOProcessor::iterator AsyncTCPIOProcessor::begin(void)
{
	return (AsyncTCPIOProcessor::iterator(_wrbuf.begin()));
}

AsyncTCPIOProcessor::iterator AsyncTCPIOProcessor::end(void)
{
	return (AsyncTCPIOProcessor::iterator(_wrbuf.end()));
}

AsyncTCPIOProcessor::fdIterator::fdIterator()
{
}

AsyncTCPIOProcessor::fdIterator::~fdIterator()
{
}

AsyncTCPIOProcessor::fdIterator::fdIterator(const fdIterator &orig)
{
	operator=(orig);
}

AsyncTCPIOProcessor::fdIterator &AsyncTCPIOProcessor::fdIterator::operator=(
	const AsyncTCPIOProcessor::fdIterator &orig)
{
	_it = orig._it;
	return (*this);
}

AsyncTCPIOProcessor::fdIterator::fdIterator(
	const AsyncTCPIOProcessor::_iterator &it)
{
	_it = it;
}

AsyncTCPIOProcessor::fdIterator &AsyncTCPIOProcessor::fdIterator::operator++(
	void)
{
	_it++;
	return (*this);
}

const AsyncTCPIOProcessor::fdIterator AsyncTCPIOProcessor::fdIterator::
operator++(int)
{
	AsyncTCPIOProcessor::fdIterator temp(*this);
	++(*this);
	return (temp);
}

bool AsyncTCPIOProcessor::fdIterator::operator!=(
	const AsyncTCPIOProcessor::fdIterator &other) const
{
	return (_it != other._it);
}

int AsyncTCPIOProcessor::fdIterator::operator*(void)
{
	return (_it->first);
}
