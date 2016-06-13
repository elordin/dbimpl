# set compiler flags and misc
CC = clang++

# CFLAGS = -std=c++11 -O3 -pthread -Wall -Wextra -pedantic
CFLAGS = -std=c++11 -O0 -pthread # -Wall -Wextra -pedantic
TFLAGS = -g -DTEST

LFLAGS =
OUTNAME = main

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

# file names
FILES = BufferManager/BufferManager.cpp BufferManager/BufferFrame.cpp BufferManager/HashTable.cpp \
		SlottedPages/SPSegment.cpp SlottedPages/TID.cpp SlottedPages/Record.cpp \
		BTree/BTree.cpp \
		Operators/Register.cpp Operators/HashJoin.cpp Operators/Print.cpp Operators/Projection.cpp \
		Operators/Select.cpp Operators/TableScan.cpp \
		buffertest.cpp
		#slottedTest.cpp
		#BTreeTest.cpp
SRC = $(FILES:%=$(SRCDIR)/%)
OBJ = $(FILES:%.cpp=$(OBJDIR)/%.o)


all: compile
	echo "\n\tUse \`make $(OUTNAME)\` to compile runnable.\n"

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
