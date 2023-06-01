#include "HTTP/ParsingFail.hpp"

using namespace HTTP;

ParsingFail::ParsingFail(const std::string &why) : std::runtime_error(why)
{
}

EmptyLineFound::EmptyLineFound(void) : ParsingFail("Empty line found")
{
}

InvalidFormat::InvalidFormat(void) : ParsingFail("Invalid format")
{
}

InvalidField::InvalidField(void) : ParsingFail("Invalid header field")
{
}

InvalidValue::InvalidValue(void) : ParsingFail("Invalid header value")
{
}

InvalidSize::InvalidSize(void) : ParsingFail("Invalid body size")
{
}
