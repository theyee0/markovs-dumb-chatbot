NAME := markovs_dumb_chatbot

CC := clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror

INC_DIR := inc
SRC_DIR := src
OBJ_DIR := obj

SRC_FILES := inference training translation main
OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRC_FILES)))

all: $(NAME)

debug: CFLAGS += -g -fsanitize=address
debug: $(NAME)

release: CFLAGS += -O3
release: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) -I $(INC_DIR) $(OBJ_FILES) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -I $(INC_DIR) $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

.PHONY: all clean debug release

clean:
	rm -rf $(OBJ_DIR)
