CXX			= c++
CXXFLAGS	= \
			-I../../includes \
			-Wall -Wextra -Werror \
			-std=c++98 -fsanitize=address -g \

SRCS		= \
			HTTPServer.cpp \
			HTTPLocation.cpp \
			HTTPServerConstValues.cpp \
			../ConfigParser/ConfigDirective.cpp \
			../ConfigParser/parseConfig.cpp \
			../ConfigParser/recursiveParser.cpp \
			../ConfigParser/splitIntoTokens.cpp \

OBJS		= $(SRCS:.cpp=.o)

all: test_constructor

test_constructor: $(OBJS) test_constructor.o
	$(CXX) $(CXXFLAGS) $(OBJS) test_constructor.o -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) test_constructor.o

fclean: clean
	$(RM) test_constructor

re:
	make fclean
	make all

.PHONY: all clean fclean re
