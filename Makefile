CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lm

# Source files
SRC = existence.c utils.c eviction.c
OBJ = $(SRC:.c=.o)
EXEC = test

# Build target
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LIBS)

# Compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(EXEC)
