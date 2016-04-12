# set compiler flags and misc
CC = g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic -O0
LFLAGS =
OUTDIR = bin
OUTNAME = main

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin
SRC = sorting/mergesort.cpp
OBJ = $(notdir $(SRC:%.cpp=%.o))

all: runnable

runnable: $(OUTNAME)

compile: $(OBJDIR)/$(OBJ)

$(OUTNAME): $(OBJDIR)/$(OBJ)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $(OBJDIR)/$(OBJ)

$(OBJDIR)/$(OBJ): $(SRCDIR)/$(SRC)
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f ./obj/*
	rm -f ./bin/*
