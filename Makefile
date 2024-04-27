CC=gcc
CFLAGS=-Wall -fPIC -Wextra -Werror -pedantic -ggdb

heap: main.c heap.h hblock.h flist.h mem.h
	$(CC) $(CFLAGS) -o  heap main.c  heap.h hblock.h flist.h mem.h