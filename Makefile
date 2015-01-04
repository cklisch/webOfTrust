CC = gcc
CFLAGS = -Wall -Wextra -ansi -std=c99
test: testmain.o trustWeb.o
	$(CC) -o test testmain.o trustWeb.o -lm

testmain.o: testmain.c trustWeb.h
	$(CC) $(CFLAGS) -c testmain.c

trustWeb.o: trustWeb.c trustWeb.h
	$(CC) $(CFLAGS) -c trustWeb.c