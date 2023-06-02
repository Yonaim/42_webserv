#include "async/TCPIOProcessor.hpp"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace async;

std::queue<int> TCPIOProcessor::disconnected_clients;

TCPIOProcessor::TCPIOProcessor(const int port, const int backlog)
	: _port(port), _backlog_size(backlog),
	  _logger(Logger::getLogger("TCPIOProcessor"))
{
}

TCPIOProcessor::~TCPIOProcessor()
{
	finalize(NULL);
}

TCPIOProcessor::TCPIOProcessor(const TCPIOProcessor &orig)
	: _port(orig._port), _backlog_size(orig._backlog_size),
	  _listening_socket(orig._listening_socket), _logger(orig._logger)
{
}

TCPIOProcessor &TCPIOProcessor::operator=(const TCPIOProcessor &orig)
{
	_port = orig._port;
	_backlog_size = orig._backlog_size;
	_listening_socket = orig._listening_socket;
	return (*this);
}

void TCPIOProcessor::task(void)
{
	flushKQueue();
	_status = status::OK_AGAIN;
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
			{
				finalize("Error from server socket");
				return;
			}
			else
			{
				_logger << async::warning << "Error from client socket";
				disconnect(ident);
			}
		}
		else if (filter == EVFILT_READ)
		{
			if (static_cast<int>(ident) == _listening_socket)
			{
				accept();
				continue;
			}

			if (flags & EV_EOF)
			{
				_logger << async::verbose << "client " << ident
						<< " reports EOF";
				disconnect(ident);
				_status = status::OK_AGAIN;
				continue;
			}
			int rc = read(ident, data);
			if (rc == status::ERROR_FILECLOSED)
			{
				_logger << async::verbose << "client " << ident << " is closed";
				disconnect(ident);
				_status = status::OK_AGAIN;
				continue;
			}
			if (rc >= status::ERROR_GENERIC)
			{
				_logger << async::warning << "Error while reading from client "
						<< ident << ": " << _error_msg;
				disconnect(ident);
				_status = status::OK_AGAIN;
				continue;
			}
		}
		else if (filter == EVFILT_WRITE)
		{
			if (_wrbuf[ident].length() > 0)
			{
				if (write(ident, _wrbuf[ident].length())
					>= status::ERROR_GENERIC)
				{
					_logger << async::warning
							<< "Error while writing to client " << ident << ": "
							<< _error_msg;
					_status = status::OK_AGAIN;
				}
			}
		}
	}
	_status = status::OK_AGAIN;
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
	_logger << async::info << "Created socket " << _listening_socket;

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);
	result = bind(_listening_socket, (struct sockaddr *)&addr,
				  sizeof(struct sockaddr_in));
	if (result < 0)
		finalize(strerror(errno));
	_logger << async::info << "Bind socket " << _listening_socket << " at port "
			<< _port;

	result = listen(_listening_socket, _backlog_size);
	if (result < 0)
		finalize(strerror(errno));
	_logger << async::info << "Listen with backlog size " << _backlog_size;

	result = fcntl(_listening_socket, F_SETFL, O_NONBLOCK);
	if (result < 0)
		finalize(strerror(errno));
	_watchlist.push_back(constructKevent(_listening_socket, IOEVENT_READ));
	flushKQueue();
	_logger << async::verbose << "TCPIOProcessor initialization complete";
}

void TCPIOProcessor::finalize(const char *with_error)
{
	if (_listening_socket >= 0)
	{
		_logger << async::verbose << "Finalize TCPIOProcessor";
		while (!_wrbuf.empty())
			disconnect(_wrbuf.begin()->first);
		close(_listening_socket);
		if (with_error)
			throw(std::runtime_error(std::string("Error from TCPIOProcessor: ")
									 + with_error));
	}
	_listening_socket = -1;
}

void TCPIOProcessor::accept(void)
{
	int new_client_socket = ::accept(_listening_socket, NULL, NULL);
	if (new_client_socket < 0)
		finalize(strerror(errno));
	_logger << async::info << "Accepted new client: " << new_client_socket;
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
	_logger << async::info << "Disconnected " << client_socket;
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
