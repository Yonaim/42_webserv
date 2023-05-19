# -------------------------------- directory --------------------------------- #

DIR_SRCS 			= srcs/
DIR_OBJS 			= objs/

DIR_ASYNC_IO 		= AsyncIOProcessor/
DIR_ASYNCFILE		= AsyncFileIOProcessor/
DIR_ASYNCLOGGER		= AsyncLogger/
DIR_CONFIG_PARSER 	= ConfigParser/
DIR_HTTP 			= HTTP/
DIR_WEBSERVER		= WebServer/

DIR_TESTSRCS 		= test/testsrcs/
DIR_TESTOBJS 		= test/objs/

DIR_TESTCASE 		= test/testcase/

# -------------------- driver source (with main function) -------------------- #

DRIVERNAMES			= main \

DRIVERSRCS			= $(addprefix $(DIR_SRCS), $(addsuffix .cpp, $(DRIVERNAMES)))
DRIVEROBJS			= $(addprefix $(DIR_OBJS), $(addsuffix .o, $(DRIVERNAMES)))

TESTDRIVERNAMES		=	\
					test_asyncio_echo \
					test_asyncio_singleio \
					test_asynclogger \
					test_asyncfilereader \
					test_asyncfilewriter \
					test_configparser \
					test_http_request \
					test_http_response \
					test_http_server_constructor \
					test_bidimap \

TESTDRIVERSRCS		= $(addprefix $(DIR_TESTSRCS), $(addsuffix .cpp, $(TESTDRIVERNAMES)))
TESTDRIVEROBJS		= $(addprefix $(DIR_TESTOBJS), $(addsuffix .o, $(TESTDRIVERNAMES)))
TESTDRIVERDEPS		= $(addprefix $(DIR_TESTOBJS), $(addsuffix .d, $(TESTDRIVERNAMES)))

# ---------------------- source (without main function) ---------------------- #

FILENAMES			= \
					$(DIR_ASYNC_IO)AsyncIOProcessor \
					$(DIR_ASYNC_IO)AsyncIOException \
					$(DIR_ASYNC_IO)AsyncSingleIOProcessor \
					$(DIR_ASYNC_IO)AsyncTCPIOProcessor \
					$(DIR_ASYNC_IO)AsyncIOTaskHandler \
					$(DIR_ASYNCFILE)AsyncFileIOProcessor \
					$(DIR_ASYNCFILE)AsyncFileReader \
					$(DIR_ASYNCFILE)AsyncFileWriter \
					$(DIR_ASYNCLOGGER)AsyncLogger \
					$(DIR_ASYNCLOGGER)EndMarker \
					$(DIR_CONFIG_PARSER)ConfigDirective \
					$(DIR_CONFIG_PARSER)parseConfig \
					$(DIR_CONFIG_PARSER)recursiveParser \
					$(DIR_CONFIG_PARSER)splitIntoTokens \
					$(DIR_HTTP)const_values \
					$(DIR_HTTP)mime_type \
					$(DIR_HTTP)utils \
					$(DIR_HTTP)Header/Header \
					$(DIR_HTTP)Request/Request \
					$(DIR_HTTP)Request/RequestConsume \
					$(DIR_HTTP)Response/Response \
					$(DIR_HTTP)Response/ResponseInit \
					$(DIR_HTTP)Response/ResponseSetter \
					$(DIR_HTTP)Server/Location \
					$(DIR_HTTP)Server/Server \
					$(DIR_HTTP)Server/ServerParseDirective \
					$(DIR_WEBSERVER)WebServer \

SRCS				= $(addprefix $(DIR_SRCS), $(addsuffix .cpp, $(FILENAMES)))
OBJS				= $(addprefix $(DIR_OBJS), $(addsuffix .o, $(FILENAMES)))
DEPS				= $(addprefix $(DIR_OBJS), $(addsuffix .d, $(FILENAMES)))