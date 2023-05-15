SRC := main.cpp utils.cpp HttpReq.cpp # HttpRes.cpp
OBJ_DIR := objs
OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
NAME := http_converter
CFLAGS := -Wall -Werror -Wextra -std=c++98 -MMD

all : $(NAME)

$(OBJ_DIR) :
	mkdir $@

$(NAME) : $(OBJ_DIR) $(OBJ)
	$(CXX) $(OBJ) -o $@

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re : fclean
	make all
