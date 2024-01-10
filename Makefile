# Makefile for Supermarket Product Shelving Simulation

CC = gcc
CFLAGS = -Wall -pthread -lpthread  -g

# List of source files
SRCS = main.c fileReaders.c team.c customers.c 

# List of object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = exec

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean