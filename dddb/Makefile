# Compiler and Flags
CC := gcc
CFLAGS := -g

# Libraries
LIBS :=  #-levent

# Source Files
SRCS := src/main.c \
        src/store/store.c \
        src/helpers/helpers.c \
        src/realtime/realtime.c \
        src/server/server.c \
        src/server/utils.c \
        src/auth/utils.c \
        lib/cjson/cJSON.c

# Target Executable
TARGET := dddb

# Default Target
all: $(TARGET)

# Compile and Link
$(TARGET):
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(TARGET)

# Run the server
run: $(TARGET)
	./$(TARGET)

# Debug with GDB
debug: $(TARGET)
	gdb ./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)

.PHONY: all clean run debug
