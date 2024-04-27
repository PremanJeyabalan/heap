
#include "mem.h"

#define PAGE_SIZE 4096
#define MAX_HEAP PAGE_SIZE*2

static char* mem_heap;
static char* mem_brk;
static char* mem_max_addr;

void mem_init() {
    mem_heap = (char*) malloc(MAX_HEAP);
    mem_brk = mem_heap;
    mem_max_addr = mem_heap + MAX_HEAP;
}

void* mem_sbrk(size_t increment) {
    char* old = mem_brk;

    if ((mem_brk + increment) > mem_max_addr)
        return (void*) -1;

    mem_brk += increment;
    return (void*) old;
}