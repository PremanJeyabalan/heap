#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define DWORD_SIZE 8

struct HeaderBlock {
    uint64_t size_free_pack;
    struct HeaderBlock* next;
    struct HeaderBlock* prev;
};

typedef struct HeaderBlock HeaderBlock;

#define HEADER_SIZE sizeof(HeaderBlock)

size_t get_block_size(const HeaderBlock* block) {
    return (block->size_free_pack >> 1);
}

size_t get_block_free(const HeaderBlock* block) {
    return (block->size_free_pack & 0x01);
}

void set_block_size(HeaderBlock* block, size_t size) {
    block->size_free_pack = ((size % 2 ? size + 1 : size) << 1) | (block->size_free_pack & 0x01);
    block->size_free_pack <<= 1;
}

void set_block_free(HeaderBlock* block, HeaderBlock* next, HeaderBlock* prev) {
    block->size_free_pack |= 0x01;
    block->next = next;
    block->prev = prev;
}

void set_block_alloc(HeaderBlock* block) {
    block->size_free_pack &= 0xFFFFFFFFFFFFFFFE;
}

void print_block(const HeaderBlock* block) {
    printf("Address: %p | Size: %ld | Free: %ld | Previous: %p | Next: %p |\n", 
        (void*) block, get_block_size(block), get_block_free(block), (void*) block->prev, (void*) block->next);
}

typedef struct {
    void* memory;
    size_t size;
    size_t capacity;
    HeaderBlock* freeList;
} Heap;

static Heap heap;

void make_heap(size_t bytes) {
    if ((heap.memory = sbrk(HEADER_SIZE)) == (void*) -1) {
        printf("Failed to allocated: %ld bytes to the heap\n", bytes);
        exit(1);
    }

    heap.capacity = bytes;
    heap.freeList = (HeaderBlock*) heap.memory;
    set_block_size(heap.freeList, bytes);
    set_block_free(heap.freeList, NULL, NULL);
}

void print_heap() {
    HeaderBlock* curr = heap.memory;
    HeaderBlock* end = curr + heap.capacity;
    while (curr < end) {
        print_block(curr);
        curr += get_block_size(curr);
    }
}

void print_heap_free_blocks() {
    HeaderBlock* curr = heap.freeList;
    while (curr != NULL) {
        print_block(curr);
        curr = curr->next;
    }
}

void* ff_malloc(size_t bytes) {
    size_t allocSize = bytes + HEADER_SIZE;
    if (heap.size + allocSize > heap.capacity) {
        printf("Requested %ld bytes but only %ld remaining in capacity %ld. reallocating...\n", bytes, heap.capacity - heap.size, heap.capacity);
        return NULL;
    }

    HeaderBlock* currFree = heap.freeList;
    while (currFree != NULL) {
        if (get_block_size(currFree) >= allocSize) {
            size_t remSize = allocSize - get_block_size(currFree);
            if (remSize > 0) {
                HeaderBlock* remFree = currFree + allocSize;
                set_block_size(remFree, remSize + HEADER_SIZE);
                set_block_free(remFree, currFree->next, currFree->prev);
            }

            set_block_alloc(currFree);
            set_block_size(currFree, allocSize);
        }
    }
}

void ff_free(void* ptr) {
    printf("Trying to free ptr %p bytes...\n", ptr);

    
}

#define ALLOCS 10

void* ptrs[ALLOCS] = {0};

int main() {

    make_heap(DWORD_SIZE*8);
    // print_heap_free_blocks();
    print_heap();

    char word[] = "This is a sentence";
    char* wordHeap = ff_malloc(sizeof(word));
    printf("Data from heap: %s\n", wordHeap);
    

    // for (int i = 0; i < ALLOCS; i++) {
    //     ptrs[i] = ff_malloc(i);
    // }

    // for (int i = 0; i < ALLOCS; i++) {
    //     ff_free(ptrs[i]);
    // }


    return 0;
}