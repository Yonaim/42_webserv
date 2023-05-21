NAME		= webserv

RM			= rm -rf

CXX			= c++
CXXFLAGS	= \
			-Wall -Wextra -Werror \
			-MMD -MP -std=c++98 -fsanitize=address -g $(ACXXFLAGS) \

CPPFLAGS	= \
				-I./includes

include filenames.mk

# ------------------------------- make rules --------------------------------- #

all: $(NAME)

objs:
	@make -j4 $(OBJS)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(DIR_TESTOBJS)%.o: $(DIR_TESTSRCS)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(NAME): objs $(DIR_OBJS)main.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(DIR_OBJS)main.o -o $@ $(LDFLAGS)

test_asyncio_echo: objs $(DIR_TESTOBJS)test_asyncio_echo.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncio_echo.o -o $@ $(LDFLAGS)

test_asyncio_singleio: objs $(DIR_TESTOBJS)test_asyncio_singleio.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncio_singleio.o -o $@ $(LDFLAGS)

test_asynclogger: objs $(DIR_TESTOBJS)test_asynclogger.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asynclogger.o -o $@ $(LDFLAGS)

test_asyncfilereader: objs $(DIR_TESTOBJS)test_asyncfilereader.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncfilereader.o -o $@ $(LDFLAGS)

test_asyncfilewriter: objs $(DIR_TESTOBJS)test_asyncfilewriter.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncfilewriter.o -o $@ $(LDFLAGS)

test_configparser: objs $(DIR_TESTOBJS)test_configparser.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_configparser.o -o $@ $(LDFLAGS)

test_http_request: objs $(DIR_TESTOBJS)test_http_request.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_request.o -o $@ $(LDFLAGS)

test_http_response: objs $(DIR_TESTOBJS)test_http_response.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_response.o -o $@ $(LDFLAGS)

test_http_server_constructor: objs $(DIR_TESTOBJS)test_http_server_constructor.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_server_constructor.o -o $@ $(LDFLAGS)

test_bidimap: objs $(DIR_TESTOBJS)test_bidimap.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_bidimap.o -o $@ $(LDFLAGS)

-include $(DEPS) $(TESTDRIVERDEPS)

clean:
	$(RM) $(DIR_OBJS) $(DIR_TESTOBJS)

fclean: clean
	$(RM) $(NAME) $(TESTDRIVERNAMES)

re: fclean
	@make all

.PHONY: all clean fclean re
