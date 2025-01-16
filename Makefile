# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wextra -Werror -I/usr/include/SDL2 -D_REENTRANT

# Linker flags
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm -ltmx -lxml2 -lz

# Sources
SRCS = main.c player.c enemy.c init.c utils.c ui.c maploader.c items.c display.c inventory.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = game

# All target
all: $(EXEC)

# Link object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.c game.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f $(OBJS)

# Clean everything except the original files
fclean: clean
	rm -f $(EXEC)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re
