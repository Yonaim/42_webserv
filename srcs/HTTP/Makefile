CXX			= c++
CXXFLAGS	= \
			-I../../includes \
			-Wall -Wextra -Werror \
			-std=c++98 -fsanitize=address -g \

SRCS	= \
		Request/HTTPRequest.cpp \
		Response/HTTPResponse.cpp \
		Response/HTTPResponseInit.cpp \
		Response/HTTPResponseSetter.cpp \
		Header/HTTPHeader.cpp \
		const_values.cpp \
		utils.cpp \

OBJS	= $(SRCS:.cpp=.o)

all: test_http_request test_http_response

test_http_request: $(OBJS) test_http_request.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_http_request.o -o $@ $(LDFLAGS)

test_http_response: $(OBJS) test_http_response.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_http_response.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_http_request.o test_http_response.o

fclean: clean
	$(RM) test_http_request test_http_response

re:
	make fclean
	make all

.PHONY: all clean fclean re
