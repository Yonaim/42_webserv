#ifndef ASYNC_FILEIOHANDLER_HPP
#define ASYNC_FILEIOHANDLER_HPP

#include "async/SingleIOProcessor.hpp"
#include "utils/shared_ptr.hpp"
#include <cstdio>
#include <ctime>
#include <string>

namespace async
{
class FileIOHandler
{
  protected:
	ft::shared_ptr<SingleIOProcessor> _processor;
	FILE *_stream;
	int _fd;
	const std::string _path;
	int _status;
	std::string _error_msg;
	std::string _buffer;
	const clock_t _timeout;
	clock_t _next_timeout;
	const bool _should_close; // 소멸자 호출시 fd를 close()해야하는지 여부

	FileIOHandler(unsigned int timeout_ms, int fd);
	FileIOHandler(unsigned int timeout_ms, const std::string &path);

	void renewTimeout(void);
	bool checkTimeout(void);
	bool openFdByPath(const char *mode);

  public:
	virtual ~FileIOHandler();

	virtual int task(void) = 0;
	const std::string &errorMsg(void) const;
	std::string retrieve(void);
};

class FileWriter : public FileIOHandler
{
  private:
	const std::string _content;

  public:
	FileWriter(unsigned int timeout_ms, int fd, const std::string &content);
	FileWriter(unsigned int timeout_ms, const std::string &path,
			   const std::string &content);
	virtual ~FileWriter();

	virtual int task(void);
};

class FileReader : public FileIOHandler
{
  private:
	bool _is_fifo;
	size_t _filesize;

  public:
	FileReader(unsigned int timeout_ms, int fd, bool is_fifo);
	FileReader(unsigned int timeout_ms, const std::string &path);
	virtual ~FileReader();

	virtual int task(void);
};
} // namespace async

#endif
