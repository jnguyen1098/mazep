CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -Wextra -ggdb3 -fopenmp -I.
VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full

.PHONY: all run runp valgrind valgrindp clean FORCE


all: maze mazep


run: all
	./maze

runp: all
	./mazep


valgrind: all
	valgrind $(VFLAGS) ./maze

valgrindp: all
	valgrind $(VFLAGS) ./mazep


maze: maze.o
	$(CC) $(CFLAGS) maze.o -o maze

mazep: mazep.o
	$(CC) $(CFLAGS) mazep.o -o mazep 


maze.o: maze.c FORCE
	$(CC) $(CFLAGS) -c maze.c -o maze.o -DSERIAL

mazep.o: maze.c FORCE
	$(CC) $(CFLAGS) -c maze.c -o mazep.o


clean:
	rm -rf *.o mazep maze


FORCE:
