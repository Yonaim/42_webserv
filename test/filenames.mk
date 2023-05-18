# -------------------------------- directory --------------------------------- #

DIR_SRCS 			= ../srcs

DIR_ASYNC_IO 		= $(DIR_SRCS)/AsyncIOProcessor
DIR_CONFIG_PARSER 	= $(DIR_SRCS)/ConfigParser
DIR_HTTP 			= $(DIR_SRCS)/HTTP

DIR_TESTSRCS 		= testsrcs
DIR_TESTCASE 		= testcase

# ----------------------------------- test ----------------------------------- #

TEST_LIST			=	\
						test_asyncio_echo \
						test_asyncio_singleio \
						test_configparser \
						test_http_request \
						test_http_response \
						test_http_server_constructor \

TEST_SRCS_ALL		= $(addsuffix .cpp, $(addprefix $(DIR_TESTSRCS)/, $(TEST_LIST)))

# ---------------------------------- source ---------------------------------- #

SRCS_ASYNC_IO		= \
						$(DIR_ASYNC_IO)/AsyncIOProcessor.cpp \
						$(DIR_ASYNC_IO)/AsyncSingleIOProcessor.cpp \
						$(DIR_ASYNC_IO)/AsyncTCPIOProcessor.cpp \
						$(DIR_ASYNC_IO)/AsyncIOTaskHandler.cpp \

SRCS_CONFIG_PARSER	= \
						$(DIR_CONFIG_PARSER)/ConfigDirective.cpp \
						$(DIR_CONFIG_PARSER)/parseConfig.cpp \
						$(DIR_CONFIG_PARSER)/recursiveParser.cpp \
						$(DIR_CONFIG_PARSER)/splitIntoTokens.cpp \

SRCS_HTTP_COMMON 	= \
						$(DIR_HTTP)/const_values.cpp \
						$(DIR_HTTP)/utils.cpp \

SRCS_HTTP_HEADER	= \
						$(DIR_HTTP)/Header/Header.cpp \

SRCS_HTTP_REQUEST	= \
						$(DIR_HTTP)/Request/Request.cpp \
						$(DIR_HTTP)/Request/RequestConsume.cpp \

SRCS_HTTP_RESPONSE	= \
						$(DIR_HTTP)/Response/Response.cpp \
						$(DIR_HTTP)/Response/ResponseInit.cpp \
						$(DIR_HTTP)/Response/ResponseSetter.cpp \

SRCS_HTTP_SERVER 	= \
						$(DIR_HTTP)/Server/Location.cpp \
						$(DIR_HTTP)/Server/Server.cpp \
						$(DIR_HTTP)/Server/ServerParseDirective.cpp \

SRCS_ALL			= \
						$(SRCS_ASYNC_IO) \
						$(SRCS_HTTP_COMMON) \
						$(SRCS_HTTP_HEADER) \
						$(SRCS_HTTP_REQUEST) \
						$(SRCS_HTTP_RESPONSE) \
						$(SRCS_HTTP_SERVER) \
						$(SRCS_TEST_FILE) \
