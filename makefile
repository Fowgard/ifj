# Autor: Jakub Gajdošík, FIT

CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -pedantic
#DEPS = scanner.h parser.h stack.h token.h symtable.h priority.h
#OBJ = scanner parser stack token symtable priority


ifj:  token.o parser.o scanner.o stack.o generator.o symtable.o priority.o main.c
	$(CC) $(CFLAGS) $^ -o $@
#%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#ifj: $(OBJ) libmylib.a
#	$(CC) -o $@ $^ $(CFLAGS) -L. -lmylib
 