# Created by Matt Berry
# Compiles and links the Terminal Blackjack program

CC=g++
CFLAGS=-std=c++17 -Wall -Isrc

SRCS = src/main.cpp \
       src/card/card.cpp \
       src/card/deck.cpp \
       src/card/hand.cpp \
       src/player/player.cpp \
       src/player/dealer.cpp \
       src/game/game.cpp \
       src/io/persistence.cpp \
       src/io/inputHandler.cpp \
       src/io/outputHandler.cpp

OBJDIR = build
OBJS = $(patsubst src/%.cpp, $(OBJDIR)/%.o, $(SRCS))

TARGET = terminal_blackjack


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
