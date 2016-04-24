# set compiler flags and misc
CC = g++

# CFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3
CFLAGS = -std=c++11 -Wall -Wextra -pedantic -O0
TFLAGS = -g -pthread -DTEST

LFLAGS =
OUTNAME = main

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

# file names
FILES = sorting/mergesort.cpp BufferManager/BufferManager.cpp \
		BufferManager/BufferFrame.cpp BufferManager/HashTable.cpp
SRC = $(FILES:%=$(SRCDIR)/%)
OBJ = $(FILES:%.cpp=$(OBJDIR)/%.o)


all: compile

compile: $(OBJ)

$(OUTNAME): $(OBJ)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/$@ $^
	rm -rf ./obj

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf ./obj
	rm -f ./bin/*
