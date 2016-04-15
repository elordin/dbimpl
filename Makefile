# set compiler flags and misc
CC = g++

GTEST_DIR = /opt/gtest

# CFLAGS = -std=c++11 -Wall -Wextra -Werror -isystem $(GTEST_DIR)/include -pedantic -O0
CFLAGS = -std=c++11 -Wall -Wextra -isystem $(GTEST_DIR)/include -pedantic -O0

TFLAGS = -g -pthread -DTEST

LFLAGS =
OUTNAME = main

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin
TESTDIR = ./test

# file names
SRC = ./src/sorting/mergesort.cpp
OBJFILES = $(notdir $(SRC:%.cpp=%.o))
OBJ = $(OBJFILES:%=$(OBJDIR)/%)

TESTSRC = ./test/all_test.cpp
TESTOBJFILES = $(notdir $(TESTSRC:%.cpp=%.o))
TESTOBJ = $(TESTOBJFILES:%=$(OBJDIR)/%)

all: compile

run: clean runnable
	./gen nums 20
	./bin/main nums out 1


runnable: $(OUTNAME)

compile: $(OBJ)


$(TESTOBJ): $(TESTSRC)
	$(CC) $(CFLAGS) $(TFLAGS) -o $@ -c $^

# TODO Don't always delete the *.o files

test: $(OBJ) $(TESTOBJ) $(GTEST_DIR)/lib/gtest_main.a
	$(CC) $(CFLAGS)  $(TFLAGS) $(LFLAGS) -lpthread $^ -o $(BINDIR)/test
	rm -rf ./obj


$(OUTNAME): $(OBJ)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/$@ $^
	rm -rf ./obj

$(OBJ): $(SRC)
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf ./obj
	rm -f ./bin/*
