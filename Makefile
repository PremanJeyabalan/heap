CC=gcc
CFLAGS=-Wall -fPIC -Wextra -Werror -pedantic -ggdb

heap: main.c heap.h hblock.h
	$(CC) $(CFLAGS) -o  heap main.c  heap.h hblock.h