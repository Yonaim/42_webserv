#ifndef HTTP_PARSINGFAIL_HPP
#define HTTP_PARSINGFAIL_HPP

#include <stdexcept>
#include <string>

namespace HTTP
{
class ParsingFail : public std::runtime_error
{
  public:
	ParsingFail(const std::string &why);
};
class EmptyLineFound : public ParsingFail
{
  public:
	EmptyLineFound(void);
};
class InvalidFormat : public ParsingFail
{
  public:
	InvalidFormat(void);
};
class InvalidField : public ParsingFail
{
  public:
	InvalidField(void);
};
class InvalidValue : public ParsingFail
{
  public:
	InvalidValue(void);
};
class InvalidSize : public ParsingFail
{
  public:
	InvalidSize(void);
};
} // namespace HTTP

#endif
