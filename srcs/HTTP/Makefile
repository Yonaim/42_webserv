CXX			= c++
CXXFLAGS	= \
			-I../../includes \
			-Wall -Wextra -Werror \
			-std=c++98 -fsanitize=address -g \

SRCS	= \
		Request/HTTPRequest.cpp \
		Request/utils.cpp \
		Header/HTTPHeader.cpp \

OBJS	= $(SRCS:.cpp=.o)

all: test_http_request

test_http_request: $(OBJS) test_http_request.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_http_request.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_http_request.o

fclean: clean
	$(RM) test_http_request

re:
	make fclean
	make all

.PHONY: all clean fclean re
