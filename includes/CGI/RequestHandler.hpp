#ifndef CGI_REQUESTHANDLER_HPP
#define CGI_REQUESTHANDLER_HPP

#include "CGI/Request.hpp"
#include "CGI/Response.hpp"
#include "HTTP/Request.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"

namespace CGI
{
class Handler
{
  private:
	async::FileReader *_reader;
	async::FileWriter *_writer;
	Response _response;

  public:
	enum response_status_e
	{
		CGI_RESPONSE_STATUS_OK = 0,
		CGI_RESPONSE_STATUS_AGAIN
	};

	Handler(const Request &request);
	~Handler();

	int task(void);
	const CGI::Response &retrieve(void);
};
} // namespace CGI

#endif
