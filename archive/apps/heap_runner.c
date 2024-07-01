//
// Created by prema on 29/4/2024.
//

#include <string.h>
#include <unistd.h>

#include "heep/heap.h"

#define ALLOCS 12


char* ptrs[ALLOCS] = {0};

int main() {

    heap_init(16 * 8);

    ptrs[0] = "A";
    for (int i = 1; i < ALLOCS; i++) {
        printf("Malloc Index: %d\n", i);
        ptrs[i] = heap_malloc(i + 1);
    }


    heap_print();

    for (int i = 1; i < ALLOCS; i++) {
        if (i % 2 == 1) {
            printf("Free Index: %d\n", i);
            heap_free(ptrs[i]);
            heap_print();
        }
    }

    for (int i = 1; i < ALLOCS; i++) {
        if (i % 2 == 0) {
            printf("Free Index: %d\n", i);
            heap_free(ptrs[i]);
            heap_print();
        }
    }

    heap_collect();

    return 0;
}
