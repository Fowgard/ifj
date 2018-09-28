CC=gcc
CFLAGS=-I.

ifj:main.o scanner.o
	$(CC) -o ifj main.o scanner.o