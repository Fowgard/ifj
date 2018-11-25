# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic
#DEPS = scanner.h parser.h token.h
#OBJ = scanner parser token


ifj: main.c token.o scanner.o parser.o generator.o
	$(CC) $(CFLAGS) -o $@ main.c token.o scanner.o parser.o generator.o
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#ifj: $(OBJ) libmylib.a
#	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib
