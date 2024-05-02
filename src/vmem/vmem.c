
#include "vmem.h"

#define PAGE_SIZE 4096
#define MAX_HEAP (PAGE_SIZE*1600)

static char* mem_heap = NULL;
static char* mem_brk = NULL;
static char* mem_max_addr = NULL;

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

void mem_cleanup() {
    free(mem_heap);
    mem_heap = NULL;
    mem_brk = NULL;
    mem_max_addr = NULL;
}