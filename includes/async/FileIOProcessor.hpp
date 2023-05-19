#ifndef FILEIOPROCESSOR_HPP
#define FILEIOPROCESSOR_HPP

#include "async//SingleIOProcessor.hpp"
#include <string>

namespace async
{
class FileIOProcessor
{
  protected:
	SingleIOProcessor _writer;
	int _status;
	std::string _buffer;

	FileIOProcessor(void);
	FileIOProcessor(const FileIOProcessor &orig);
	FileIOProcessor &operator=(const FileIOProcessor &orig);

  public:
	enum load_status_e
	{
		LOAD_STATUS_OK = 0,
		LOAD_STATUS_AGAIN
	};

	FileIOProcessor(int fd);
	FileIOProcessor(const std::string &path);
	virtual ~FileIOProcessor();

	// TODO: 타임아웃 구현
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
	FileWriter(int fd);
	FileWriter(const std::string &path, const std::string &content);
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
	FileReader(int fd);
	FileReader(const std::string &path);
	virtual ~FileReader();

	virtual int task(void);
};
} // namespace async

#endif
