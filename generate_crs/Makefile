#!/bin/bash

CC = g++ -std=c++11
CFLAGS = -O2
EXEC = gen_crs
OBJ1 = graph.o 

all: $(EXEC)

gen_crs: main.o $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm $(EXEC) *.o


