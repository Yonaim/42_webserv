#ifndef FILEIOPROCESSOR_HPP
#define FILEIOPROCESSOR_HPP

#include "async//SingleIOProcessor.hpp"
#include <ctime>
#include <string>

namespace async
{
class FileIOProcessor
{
  protected:
	SingleIOProcessor _writer;
	int _status;
	std::string _buffer;
	clock_t _timeout_ms;
	bool _should_close;

	FileIOProcessor(void);
	FileIOProcessor &operator=(const FileIOProcessor &orig);
	FileIOProcessor(const FileIOProcessor &orig);
	FileIOProcessor(unsigned int timeout_ms, int fd);
	FileIOProcessor(unsigned int timeout_ms, const std::string &path);

	void checkTimeout(void);

  public:
	class Timeout : public std::runtime_error
	{
	  public:
		Timeout(const int fd);
	};

	enum load_status_e
	{
		LOAD_STATUS_OK = 0,
		LOAD_STATUS_AGAIN
	};

	virtual ~FileIOProcessor();

	virtual int task(void) = 0;
	std::string retrieve(void);
};

class FileWriter : public FileIOProcessor
{
  private:
	FileWriter();
	FileWriter(const FileWriter &orig);
	FileWriter &operator=(const FileWriter &orig);

  public:
	FileWriter(unsigned int timeout_ms, int fd);
	FileWriter(unsigned int timeout_ms, const std::string &path,
			   const std::string &content);
	virtual ~FileWriter();

	virtual int task(void);
};

class FileReader : public FileIOProcessor
{
  private:
	FileReader();
	FileReader(const FileReader &orig);
	FileReader &operator=(const FileReader &orig);

  public:
	FileReader(unsigned int timeout_ms, int fd);
	FileReader(unsigned int timeout_ms, const std::string &path);
	virtual ~FileReader();

	virtual int task(void);
};
} // namespace async

#endif
