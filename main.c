#include <string.h>
#include "heap.h"

#define ALLOCS 10


void* ptrs[ALLOCS] = {0};

int main() {

    make_heap(16*8);
    print_heap();

    char message[] = "meowwwwmo! haha!";
    
    char* mPointer = ff_malloc(sizeof(message));    
    memcpy(mPointer, message, sizeof(message));

    print_heap();

    ff_malloc(12);
    print_heap();

    ff_malloc(12);
    print_heap();

    printf("Message: %s, Stored in heap: %s\n", message, mPointer);


    return 0;
}