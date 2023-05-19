CXX			= c++
CXXFLAGS	= \
				-Wall -Wextra -Werror \
				-std=c++98 -fsanitize=address -g
CPPFLAGS	= \
				-I../includes
SRCS 		=
OBJS		= $(SRCS:.cpp=.o)

TESTFILE 	=
TESTCASE 	=

include set_value.mk

# ------------------------------- make rules --------------------------------- #

test_asyncio_echo: $(OBJS) $(DIR_TESTSRCS)/test_asyncio_echo.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_asyncio_echo.o -o $@ $(LDFLAGS)

test_asyncio_singleio: $(OBJS) $(DIR_TESTSRCS)/test_asyncio_singleio.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_asyncio_singleio.o -o $@ $(LDFLAGS)

test_configparser: $(OBJS) $(DIR_TESTSRCS)/test_configparser.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_configparser.o -o $@ $(LDFLAGS)

test_http_request: $(OBJS) $(DIR_TESTSRCS)/test_http_request.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_http_request.o -o $@ $(LDFLAGS)

test_http_response: $(OBJS) $(DIR_TESTSRCS)/test_http_response.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_http_response.o -o $@ $(LDFLAGS)

test_http_server_constructor: $(OBJS) $(DIR_TESTSRCS)/test_http_server_constructor.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_http_server_constructor.o -o $@ $(LDFLAGS)

test_bidimap: $(OBJS) $(DIR_TESTSRCS)/test_bidimap.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTSRCS)/test_bidimap.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(SRCS_ALL:.cpp=.o) $(TEST_SRCS_ALL:.cpp=.o)

fclean: clean
	$(RM) $(TEST_LIST)

.PHONY: all clean fclean re
