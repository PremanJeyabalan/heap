
#ifndef HEAP_H
#define HEAP_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hblock.h"
#include "flist.h"
#include "mem.h"

typedef struct {
    void* memory;
    size_t size;
    size_t capacity;
    FreeList freeList;
} Heap;

void* ff_malloc(size_t bytes);
void ff_free(void* ptr);
void make_heap(size_t bytes);
void print_heap();

#endif