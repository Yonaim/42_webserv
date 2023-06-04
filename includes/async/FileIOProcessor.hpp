#ifndef ASYNC_FILEIOPROCESSOR_HPP
#define ASYNC_FILEIOPROCESSOR_HPP

#include "async/SingleIOProcessor.hpp"
#include <cstdio>
#include <ctime>
#include <string>

namespace async
{
/**
 * @class FileIOProcessor
 * @brief 주어진 파일에 대하여 논블로킹한 방식으로 비동기 입출력을 처리하는
 *        추상 클래스
 * 
 * FileIOProcessor는 파일의 스트림을 관리하고 SingleIOProcessor 클래스로 파일에
 * 대한 읽기 및 쓰기 작업을 수행한다. 작업에 걸리는 시간을 _timeout으로
 * 제한하여 제한 시간을 초과하면 타임 아웃으로 처리하고 관련 에러 메시지를
 * 저장한다.
 */
class FileIOProcessor
{
  protected:
	SingleIOProcessor *_processor;
	FILE *_stream;
	int _fd;
	const std::string _path;
	int _status;
	std::string _error_msg;
	std::string _buffer;
	const clock_t _timeout;
	clock_t _next_timeout;
	const bool _should_close; // 소멸자 호출시 fd를 close()해야하는지 여부

	FileIOProcessor(unsigned int timeout_ms, int fd);
	FileIOProcessor(unsigned int timeout_ms, const std::string &path);

	void renewTimeout(void);
	bool checkTimeout(void);
	bool openFdByPath(const char *mode);

  public:
	virtual ~FileIOProcessor();

	virtual int task(void) = 0;
	const std::string &errorMsg(void) const;
	std::string retrieve(void);
};

/**
 * @class FileWriter
 * @brief 주어진 파일에 대하여 쓰기 작업 수행하는 FileIOProcessor의 유도클래스
 *
 * FileWriter는 주어진 파일에 대해 논블로킹 방식으로 비동기적인 쓰기 작업을
 * 수행한다.
 */

class FileWriter : public FileIOProcessor
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

/**
 * @class FileReader
 * @brief 주어진 파일에 대하여 읽기 작업을 수행하는 FileIOProcessor의 유도클래스
 *
 * FileReader는 주어진 파일에 대해 논블로킹 방식으로 비동기적인 읽기 작업을
 * 수행한다.
 */
class FileReader : public FileIOProcessor
{
  private:
	bool _is_fifo;

  public:
	FileReader(unsigned int timeout_ms, int fd, bool is_fifo);
	FileReader(unsigned int timeout_ms, const std::string &path);
	virtual ~FileReader();

	virtual int task(void);
};
} // namespace async

#endif
