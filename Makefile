# Created by Matt Berry
# Compiles and links the Terminal Blackjack program

CC=g++
CFLAGS=-std=c++17 -Wall -I.
SRCS= main.cpp \
      card.cpp \
      deck.cpp
OBJS=$(SRCS:.cpp=.o)
TARGET=terminal_blackjack

.PHONY: all clean

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
