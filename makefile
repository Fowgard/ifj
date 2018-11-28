# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic
#DEPS = scanner.h token.h
#OBJ = scanner token


ifj: main.c token.o scanner.o generator.o
	$(CC) $(CFLAGS) -o $@ main.c token.o scanner.o generator.o
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#ifj: $(OBJ) libmylib.a
#	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib