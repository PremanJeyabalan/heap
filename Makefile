CC=gcc
CFLAGS=-Wall -fPIC -Wextra -Werror -pedantic -ggdb

heap: main.c 
	$(CC) $(CFLAGS) -o  heap main.c 