#ifndef HEAP_H
#define HEAP_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hblock.h"
#include "flist.h"


typedef struct {
    void* memory;
    size_t size;
    size_t capacity;
    FreeList freeList;
} Heap;

void heap_init(size_t bytes);
void* heap_malloc(size_t bytes);
void heap_free(void* ptr);
void heap_collect();
void heap_print();
size_t heap_get_size();
size_t heap_get_capacity();
size_t heap_get_free_size();

#endif