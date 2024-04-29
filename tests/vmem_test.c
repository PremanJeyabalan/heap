//
// Created by prema on 29/4/2024.
//

#include <stdio.h>

#include "../src/vmem/vmem.c"

int test_mem_init(void) {
    mem_init();
    int res = (mem_heap == NULL ||
            mem_brk != mem_heap ||
            (mem_max_addr < mem_heap || (mem_max_addr - mem_heap != MAX_HEAP)));

    return res;
}

int test_mem_sbrk_empty() {
    mem_init();
    char* oldBrk = mem_brk;
    int res = ((void*)oldBrk != mem_sbrk(0));
    mem_cleanup();
    return res;
}

int test_mem_sbrk_valid() {
    mem_init();
    char* oldBrk = mem_brk;
    char* retBrk = mem_sbrk(64);
    int res = ((retBrk != oldBrk) || (mem_brk - oldBrk != 64));
    if (res)
        printf("Failed sbrk valid! oldBrk: %p retBrk: %p mem_brk: %p\n", (void*) oldBrk, (void*) retBrk, (void*) mem_brk);
    mem_cleanup();
    return res;
}

int test_mem_sbrk_invalid() {
    mem_init();
    char* oldBrk = mem_brk;
    void* retBrk = mem_sbrk(MAX_HEAP + 1);
    int res = ((retBrk != (void*)-1) || oldBrk != mem_brk);
    if (res)
        printf("Failed sbrk invalid! oldBrk: %p retBrk: %p mem_brk: %p\n", (void*) oldBrk, (void*) retBrk, (void*) mem_brk);

    mem_cleanup();
    return res;
}


int main(void) {
    if (test_mem_init())
        return 1;

    if (test_mem_sbrk_empty())
        return 1;

    if (test_mem_sbrk_valid())
        return 1;

    if (test_mem_sbrk_invalid())
        return 1;
}
