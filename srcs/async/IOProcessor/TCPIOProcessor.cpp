#include "async/TCPIOProcessor.hpp"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace async;

const int TCPIOProcessor::_backlog = 8;
std::queue<int> TCPIOProcessor::disconnected_clients;

TCPIOProcessor::TCPIOProcessor(const int port)
	: _port(port), _logger(Logger::getLogger("TCPIOProcessor"))
{
}

TCPIOProcessor::~TCPIOProcessor()
{
}

TCPIOProcessor::TCPIOProcessor(const TCPIOProcessor &orig)
	: _port(orig._port), _listening_socket(orig._listening_socket),
	  _logger(orig._logger)
{
}

TCPIOProcessor &TCPIOProcessor::operator=(const TCPIOProcessor &orig)
{
	_port = orig._port;
	_listening_socket = orig._listening_socket;
	return (*this);
}

void TCPIOProcessor::task(void)
{
	flushKQueue();
	while (!_eventlist.empty())
	{
		int flags = _eventlist.front().flags;
		int filter = _eventlist.front().filter;
		int ident = _eventlist.front().ident;
		int data = _eventlist.front().data;
		_eventlist.pop_front();
		if (flags & EV_ERROR)
		{
			if (static_cast<int>(ident) == _listening_socket)
				finalize("Error from server socket");
			else
			{
				_logger << "Error from client socket" << async::warning;
				disconnect(ident);
			}
		}
		else if (filter == EVFILT_READ)
		{
			if (static_cast<int>(ident) == _listening_socket)
				accept();
			else
			{
				try
				{
					read(ident, data);
				}
				catch (const std::runtime_error &e)
				{
					_logger << "Error while reading from client " << ident
							<< ": " << e.what() << async::warning;
					disconnect(ident);
				}
			}
		}
		else if (filter == EVFILT_WRITE)
		{
			if (_wrbuf[ident].length() > 0)
			{
				try
				{
					write(ident, _wrbuf[ident].length());
				}
				catch (const IOProcessor::WriteError &e)
				{
					_logger << "Error while writing to client " << ident << ": "
							<< e.what() << async::warning;
				}
			}
		}
	}
}

void TCPIOProcessor::initialize(void)
{
	int result;
	_listening_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_listening_socket < 0)
		throw(std::runtime_error(std::string("Error while creating socket: ")
								 + strerror(errno)));
	int option = 1;
	setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &option,
			   sizeof(option));
	_logger << "Created socket " << _listening_socket << async::info;

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);
	result = bind(_listening_socket, (struct sockaddr *)&addr,
				  sizeof(struct sockaddr_in));
	if (result < 0)
		finalize(strerror(errno));
	_logger << "Bind socket " << _listening_socket << " at port " << _port
			<< async::info;

	result = listen(_listening_socket, _backlog);
	if (result < 0)
		finalize(strerror(errno));
	_logger << "Listen with backlog size " << _backlog << async::info;

	result = fcntl(_listening_socket, F_SETFL, O_NONBLOCK);
	if (result < 0)
		finalize(strerror(errno));
	_watchlist.push_back(constructKevent(_listening_socket, IOEVENT_READ));
	flushKQueue();
	_logger << "TCPIOProcessor initialization complete" << async::verbose;
}

void TCPIOProcessor::finalize(const char *with_error)
{
	_logger << "Finalize TCPIOProcessor" << async::verbose;
	close(_listening_socket);
	_listening_socket = 0;
	if (with_error)
		throw(std::runtime_error(std::string("Error from TCPIOProcessor: ")
								 + with_error));
}

void TCPIOProcessor::accept(void)
{
	int new_client_socket = ::accept(_listening_socket, NULL, NULL);
	if (new_client_socket < 0)
		finalize(strerror(errno));
	_logger << "Accepted new client: " << new_client_socket << async::info;
	int result = fcntl(new_client_socket, F_SETFL, O_NONBLOCK);
	if (result < 0)
		finalize(strerror(errno));
	_watchlist.push_back(constructKevent(new_client_socket, IOEVENT_READ));
	_watchlist.push_back(constructKevent(new_client_socket, IOEVENT_WRITE));
	_rdbuf[new_client_socket] = "";
	_wrbuf[new_client_socket] = "";
}

void TCPIOProcessor::disconnect(const int client_socket)
{
	close(client_socket);
	_rdbuf.erase(client_socket);
	_wrbuf.erase(client_socket);
	disconnected_clients.push(client_socket);
	_logger << "Disconnected " << client_socket << async::info;
}

std::string &TCPIOProcessor::rdbuf(const int fd)
{
	return (_rdbuf[fd]);
}

std::string &TCPIOProcessor::wrbuf(const int fd)
{
	return (_wrbuf[fd]);
}

TCPIOProcessor::iterator TCPIOProcessor::begin(void)
{
	return (TCPIOProcessor::iterator(_wrbuf.begin()));
}

TCPIOProcessor::iterator TCPIOProcessor::end(void)
{
	return (TCPIOProcessor::iterator(_wrbuf.end()));
}

TCPIOProcessor::fdIterator::fdIterator()
{
}

TCPIOProcessor::fdIterator::~fdIterator()
{
}

TCPIOProcessor::fdIterator::fdIterator(const fdIterator &orig) : _it(orig._it)
{
}

TCPIOProcessor::fdIterator &TCPIOProcessor::fdIterator::operator=(
	const TCPIOProcessor::fdIterator &orig)
{
	_it = orig._it;
	return (*this);
}

TCPIOProcessor::fdIterator::fdIterator(const TCPIOProcessor::_iterator &it)
{
	_it = it;
}

TCPIOProcessor::fdIterator &TCPIOProcessor::fdIterator::operator++(void)
{
	_it++;
	return (*this);
}

const TCPIOProcessor::fdIterator TCPIOProcessor::fdIterator::operator++(int)
{
	TCPIOProcessor::fdIterator temp(*this);
	++(*this);
	return (temp);
}

bool TCPIOProcessor::fdIterator::operator!=(
	const TCPIOProcessor::fdIterator &other) const
{
	return (_it != other._it);
}

int TCPIOProcessor::fdIterator::operator*(void)
{
	return (_it->first);
}
