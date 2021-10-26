#!/bin/bash

CC = g++ -std=c++11
CFLAGS = -Ofast -mtune=native -march=native -mfpmath=sse -funroll-loops -fforce-addr
EXEC = schemarank
OBJ1 = graph.o schemarank.o

all: $(EXEC)

schemarank: $(OBJ1) main.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp %.hpp
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm $(EXEC) *.o


