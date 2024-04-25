CC=gcc
CFLAGS=-Wall -fPIC -Wextra -Werror -pedantic -ggdb

heapp: main.c heap.c hblock.c flist.c mem.c
	$(CC) $(CFLAGS) -o  heapp main.c heap.c hblock.c flist.c mem.c