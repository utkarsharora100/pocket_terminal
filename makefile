
CC = gcc
CXX = g++
TARGET = pocket_terminal
CFLAGS = -O2 -Wall -I. -Isrc/ui -Isrc/driver -Isrc/middleware
LDFLAGS = -lm -lgpiod -lpthread

C_SRCS = main.c $(wildcard src/ui/*.c) $(wildcard src/driver/*.c) $(wildcard src/middleware/*.c) $(shell find lvgl/src -name "*.c")
CPP_SRCS = src/ui/eez-flow.cpp
OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -delete
	rm -f $(TARGET)

