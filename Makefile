CC = gcc
CFLAGS = -Wall -Wextra -ansi -std=c99
test: testmain.o trustWeb.o web_mgmt.o
	$(CC) -o test testmain.o trustWeb.o web_mgmt.o -lm

testmain.o: testmain.c trustWeb.h
	$(CC) $(CFLAGS) -c testmain.c

trustWeb.o: trustWeb.c trustWeb.h
	$(CC) $(CFLAGS) -c trustWeb.c

web_mgmt.o: web_mgmt.c web_mgmt.h
	$(CC) $(CFLAGS) -c web_mgmt.c
