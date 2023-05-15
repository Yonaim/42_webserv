#ifndef STATUS_MODULE_HPP
#define STATUS_MODULE_HPP

#include <map>
#include <string>

// status module(singleton class)
class StatusModule
{
  public:
	static StatusModule &GetInstance();
	std::string          toStr(int status_code) const;
	std::string const   &getReasonPhrase(int status_code) const;

  private:
	typedef std::map<int, std::string>::const_iterator _iterator;

	std::map<int, std::string> _reason_phrase;

	StatusModule();
	~StatusModule();
	StatusModule(StatusModule const &other);
	StatusModule &operator=(StatusModule const &other);
	void          initReasonPhrase();
};

#endif