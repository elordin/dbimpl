# set compiler flags and misc
CC = g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic -O0
LFLAGS =
OUTDIR = bin
OUTNAME = main

SRC = ./src/sorting/mergesort.cpp

$(OUTNAME): $(SRC)
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ -c $(SRC)

clean:
	rm -f ./bin/*
