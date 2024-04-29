//
// Created by prema on 29/4/2024.
//

#include <string.h>
#include <unistd.h>

#include "heep/heap.h"

#define ALLOCS 12


char* ptrs[ALLOCS] = {0};

int main() {

    make_heap(16*8);

    ptrs[0] = "A";
    for (int i = 1; i < ALLOCS; i++) {
        printf("Malloc Index: %d\n", i);
        ptrs[i] = ff_malloc(i+1);
    }


    print_heap();

    for (int i = 1; i < ALLOCS; i++) {
        if (i % 2 == 1) {
            printf("Free Index: %d\n", i);
            ff_free(ptrs[i]);
            print_heap();
        }
    }

    for (int i = 1; i < ALLOCS; i++) {
        if (i % 2 == 0) {
            printf("Free Index: %d\n", i);
            ff_free(ptrs[i]);
            print_heap();
        }
    }

    collect_heap();

    return 0;
}
