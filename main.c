#include <string.h>

#include "heap.h"
#include <unistd.h>

#define ALLOCS 12


char* ptrs[ALLOCS] = {0};

int main() {

    make_heap(16*8);

    ptrs[0] = "A";
    for (int i = 1; i < ALLOCS; i++) {
        printf("Index: %d\n", i);
        ptrs[i] = ff_malloc(i+1);
        print_heap();
    }

    printf("%lu PAGE SIZE\n", sysconf(_SC_PAGE_SIZE));

    // printf("")
    // void* currsbrk = sbrk(0);
    // void* nextsbrk = sbrk(16);
    // void* newsbrk = sbrk(0);
    // printf("curr sbrk: %p , curr sbrk: %p , curr sbrk: %p\n", currsbrk, nextsbrk, newsbrk);


    return 0;
}