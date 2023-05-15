NAME			= webserv

INC_DIR			= -I. -Iincludes
RM				= rm -rf
CXXFLAGS		= -Wall -Werror -Wextra -MMD -MP $(ACXXFLAGS) $(INC_DIR)

DIR_SRC			= srcs/
DIR_OBJ			= objs/

FILENAME		= \
				main \
				AsyncIOProcessor/AsyncIOProcessor \
				AsyncIOProcessor/AsyncIOTaskHandler \
				AsyncIOProcessor/AsyncSingleIOProcessor \
				AsyncIOProcessor/AsyncTCPIOProcessor \
				ConfigParser/ConfigDirective \
				ConfigParser/parseConfig \
				ConfigParser/recursiveParser \
				ConfigParser/splitIntoTokens \
				HTTPServer/HTTPServer \
				HTTPServer/HTTPServerConstValues \

SRC				= $(addprefix $(DIR_SRC), $(addsuffix .cpp, $(FILENAME)))
OBJ				= $(addprefix $(DIR_OBJ), $(addsuffix .o, $(FILENAME)))
DEP				= $(addprefix $(DIR_OBJ), $(addsuffix .d, $(FILENAME)))

all: $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c -o $@ $<

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)

-include $(DEP)

clean:
	$(RM) $(DIR_OBJ)

fclean: clean
	$(RM) $(NAME)

re:
	@make fclean
	@make all

.PHONY: all bonus clean fclean re
