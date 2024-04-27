#include <string.h>
#include "heap.h"

#define ALLOCS 12


char* ptrs[ALLOCS] = {0};

int main() {

    make_heap(16*8);
    print_heap();

    ptrs[0] = "A";
    for (int i = 1; i < ALLOCS; i++) {
        printf("Index: %d\n", i);
        ptrs[i] = ff_malloc(i);
        memcpy(ptrs[i], ptrs[i-1], i);
        ptrs[i][i] = ptrs[i][i-1] + 1;
        printf("curr string: %s, index: %d\n", ptrs[i], i);
        print_heap();
    }
    // void* currsbrk = sbrk(0);
    // void* nextsbrk = sbrk(16);
    // void* newsbrk = sbrk(0);
    // printf("curr sbrk: %p , curr sbrk: %p , curr sbrk: %p\n", currsbrk, nextsbrk, newsbrk);


    return 0;
}