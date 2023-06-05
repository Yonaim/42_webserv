NAME		= webserv

RM			= rm -rf

CXX			= c++
CXXFLAGS	= \
			-Wall -Wextra -Werror \
			-MMD -MP -std=c++98 $(ACXXFLAGS) \

CPPFLAGS	= \
				-I./includes

include filenames.mk

# ------------------------------- make rules --------------------------------- #

# TODO: 제출본에서는 all: $(NAME)으로 변경
all:
	@make -j4 $(NAME)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(DIR_TESTOBJS)%.o: $(DIR_TESTSRCS)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(NAME): $(OBJS) $(DIR_OBJS)main.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(DIR_OBJS)main.o -o $@ $(LDFLAGS)

www/cgi_script/fortune.teapot: www/cgi-code/cgi_example.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) www/cgi-code/cgi_example.o -o $@ $(LDFLAGS)

test_asyncio_echo: $(OBJS) $(DIR_TESTOBJS)test_asyncio_echo.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncio_echo.o -o $@ $(LDFLAGS)

test_asyncio_singleio: $(OBJS) $(DIR_TESTOBJS)test_asyncio_singleio.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncio_singleio.o -o $@ $(LDFLAGS)

test_asynclogger: $(OBJS) $(DIR_TESTOBJS)test_asynclogger.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asynclogger.o -o $@ $(LDFLAGS)

test_asyncfilereader: $(OBJS) $(DIR_TESTOBJS)test_asyncfilereader.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncfilereader.o -o $@ $(LDFLAGS)

test_asyncfilewriter: $(OBJS) $(DIR_TESTOBJS)test_asyncfilewriter.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_asyncfilewriter.o -o $@ $(LDFLAGS)

test_configparser: $(OBJS) $(DIR_TESTOBJS)test_configparser.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_configparser.o -o $@ $(LDFLAGS)

test_http_request: $(OBJS) $(DIR_TESTOBJS)test_http_request.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_request.o -o $@ $(LDFLAGS)

test_http_response: $(OBJS) $(DIR_TESTOBJS)test_http_response.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_response.o -o $@ $(LDFLAGS)

test_http_server_constructor: $(OBJS) $(DIR_TESTOBJS)test_http_server_constructor.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_http_server_constructor.o -o $@ $(LDFLAGS)

test_bidimap: $(OBJS) $(DIR_TESTOBJS)test_bidimap.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_bidimap.o -o $@ $(LDFLAGS)

test_cgirequesthandler: $(OBJS) $(DIR_TESTOBJS)test_cgirequesthandler.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_cgirequesthandler.o -o $@ $(LDFLAGS)

test_cgi_script: $(OBJS) $(DIR_TESTOBJS)test_cgi_script.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_cgi_script.o -o $@ $(LDFLAGS)

test_shared_ptr: $(OBJS) $(DIR_TESTOBJS)test_shared_ptr.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(DIR_TESTOBJS)test_shared_ptr.o -o $@ $(LDFLAGS)

-include $(DEPS) $(TESTDRIVERDEPS)

clean:
	$(RM) $(DIR_OBJS) $(DIR_TESTOBJS) www/cgi-code/cgi_example.o www/cgi-code/cgi_example.d

fclean: clean
	$(RM) $(NAME) $(TESTDRIVERNAMES) www/cgi_script/fortune.teapot

re: fclean
	@make all

.PHONY: all clean fclean re
