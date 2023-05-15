CXX			= c++
CXXFLAGS	= \
			-I../../includes \
			-Wall -Wextra -Werror \
			-std=c++98 -fsanitize=address -g \

SRCS	= AsyncIOProcessor.cpp AsyncSingleIOProcessor.cpp AsyncTCPIOProcessor.cpp AsyncIOTaskHandler.cpp
OBJS	= $(SRCS:.cpp=.o)

all: test_singleio test_echo

test_singleio: $(OBJS) test_singleio.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_singleio.o -o $@ $(LDFLAGS)

test_echo: $(OBJS) test_echo.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_echo.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_singleio.o test_echo.o

fclean: clean
	$(RM) server client test_singleio test_echo

re:
	make fclean
	make all

.PHONY: all clean fclean re
