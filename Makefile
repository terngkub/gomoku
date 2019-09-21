
# Compiler Settings
CC			=	g++
CPP_VERSION	=	-std=c++1z
# DEBUG		=	-g3 -fsanitize=address
OPTIMIZE	=	-O3
CFLAGS		=	-Wall -Wextra -Werror $(CPP_VERSION) $(DEBUG) $(OPTIMIZE)

# Project Settings
NAME		=	gomoku
HEADER		=	board.hpp \
				index.hpp \
				flag.hpp \
				game.hpp \
				minimax.hpp
FILE		=	board.cpp \
				index.cpp \
				flag.cpp \
				game.cpp \
				main.cpp \
				minimax.cpp
SRC_DIR		=	src
INC_DIR		=	include
OBJ_DIR		=	obj

INC_FLAG	=	-I $(INC_DIR)
SRC			=	$(addprefix $(SRC_DIR)/,$(FILE))
INC			=	$(addprefix $(INC_DIR)/,$(HEADER))
OBJ			=	$(addprefix $(OBJ_DIR)/,$(FILE:%.cpp=%.o))

# Rules

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ) $(INC)
	$(CC) $(CFLAGS) $(INC_FLAG) -o $(NAME) $(OBJ)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INC_FLAG) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
