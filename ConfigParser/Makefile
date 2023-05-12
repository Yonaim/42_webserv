CXX			= c++
CXXFLAGS	= \
			-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g \

SRCS	= ConfigDirective.cpp parseConfig.cpp recursiveParser.cpp splitIntoTokens.cpp
OBJS	= $(SRCS:.cpp=.o)

all: test_parser

test_parser: $(OBJS) test_parser.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_parser.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_parser.o

fclean: clean
	$(RM) test_parser

re:
	make fclean
	make all

.PHONY: all clean fclean re
