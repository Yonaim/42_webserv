#ifndef ASYNCFILEIOPROCESSOR_HPP
#define ASYNCFILEIOPROCESSOR_HPP

#include "AsyncSingleIOProcessor.hpp"
#include <string>

class AsyncFileIOProcessor
{
  protected:
	AsyncSingleIOProcessor _writer;
	int _status;
	std::string _buffer;

	AsyncFileIOProcessor(void);
	AsyncFileIOProcessor(const AsyncFileIOProcessor &orig);
	AsyncFileIOProcessor &operator=(const AsyncFileIOProcessor &orig);

  public:
	enum load_status_e
	{
		LOAD_STATUS_OK = 0,
		LOAD_STATUS_AGAIN
	};

	AsyncFileIOProcessor(int fd);
	AsyncFileIOProcessor(const std::string &path);
	virtual ~AsyncFileIOProcessor();

	// TODO: 타임아웃 구현
	virtual int task(void) = 0;
	std::string retrieve(void);
};

class AsyncFileWriter : public AsyncFileIOProcessor
{
  private:
	AsyncFileWriter();
	AsyncFileWriter(const AsyncFileWriter &orig);
	AsyncFileWriter &operator=(const AsyncFileWriter &orig);

  public:
	AsyncFileWriter(int fd);
	AsyncFileWriter(const std::string &path, const std::string &content);
	virtual ~AsyncFileWriter();

	virtual int task(void);
};

class AsyncFileReader : public AsyncFileIOProcessor
{
  private:
	AsyncFileReader();
	AsyncFileReader(const AsyncFileReader &orig);
	AsyncFileReader &operator=(const AsyncFileReader &orig);

  public:
	AsyncFileReader(int fd);
	AsyncFileReader(const std::string &path);
	virtual ~AsyncFileReader();

	virtual int task(void);
};

#endif
