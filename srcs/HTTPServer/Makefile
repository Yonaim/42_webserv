CXX			= c++
CXXFLAGS	= \
			-I../../includes \
			-Wall -Wextra -Werror \
			-std=c++98 -fsanitize=address -g \

SRCS	= HTTPServer.cpp HTTPServerConstValues.cpp
OBJS	= $(SRCS:.cpp=.o)

all: test_server

test_server: $(OBJS) test_server.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_server.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_server.o

fclean: clean
	$(RM) test_server

re:
	make fclean
	make all

.PHONY: all clean fclean re
