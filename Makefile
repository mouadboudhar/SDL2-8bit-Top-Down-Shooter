# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wextra -Werror -I/usr/include/SDL2 -Iinclude -D_REENTRANT

# Linker flags
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm -ltmx -lxml2 -lz

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Sources
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/player.c $(SRC_DIR)/enemy.c $(SRC_DIR)/init.c \
       $(SRC_DIR)/utils.c $(SRC_DIR)/ui.c $(SRC_DIR)/maploader.c $(SRC_DIR)/items.c \
       $(SRC_DIR)/display.c $(SRC_DIR)/inventory.c

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executable name
EXEC = game

# All target
all: $(OBJ_DIR) $(EXEC)

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c include/game.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -rf $(OBJ_DIR)

# Clean everything except the original files
fclean: clean
	rm -f $(EXEC)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
