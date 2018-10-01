# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic
DEPS = scanner.h token.h
OBJ = scanner.o token.o main.o 



%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ifj: $(OBJ) libmylib.a
	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib

libmylib.a: $(OBJ) #let's link library files into a static library
	ar rcs $@ $(OBJ)

libs: libmylib.a