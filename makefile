# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic
#DEPS = scanner.h parser.h stack.h token.h symtable.h
#OBJ = scanner parser stack token symtable


ifj: main.c token.o scanner.o parser.o stack.o generator.o symtable.o
	$(CC) $(CFLAGS) -o $@ main.c token.o scanner.o parser.o stack.o generator.o symtable.o
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#ifj: $(OBJ) libmylib.a
#	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib
