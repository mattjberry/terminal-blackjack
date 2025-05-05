# Created by Matt Berry
# Compiles and links the Terminal Blackjack program

CC=g++
CFLAGS=-std=c++17 -Wall -I.
SRCS= main.cpp \
      card.cpp \
      deck.cpp

OBJDIR = build
OBJS=$(SRCS:.cpp=.o)
OBJS := $(addprefix $(OBJDIR)/, $(OBJS))

TARGET=terminal_blackjack


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)
