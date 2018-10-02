# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic
#DEPS = scanner.h token.h
#OBJ = scanner token


ifj: main.c token.o scanner.o 
	$(CC) $(CFLAGS) -o $@ main.c scanner.o token.o
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#ifj: $(OBJ) libmylib.a
#	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib