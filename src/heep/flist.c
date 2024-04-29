#include "heep/flist.h"

void list_init(FreeList* list, void* memory, size_t size) {
    list->head = (HeaderBlock*) memory;
    make_block(list->head, size, true, NULL, NULL);
    list->tail = list->head;
    list->size = size;
}

HeaderBlock* list_append(FreeList* list, void* memory, size_t size) {
    if (list->head == NULL) {
        list_init(list, memory, size);
        return list->head;
    }

    HeaderBlock* next = get_end_addr_block(list->tail);
    make_block(next, size, true, NULL, list->tail);
    list->tail->next = next;
    list->tail = list->tail->next;
    size += get_size(next);
    return list->tail;
}

void list_pop_front(FreeList* list) {
    HeaderBlock* head = list->head;
    list->size -= get_size(head);
    if (head->next == NULL) {
        list->head = NULL;
        list->tail = NULL;
        return;
    }

    list->head = list->head->next;
}

void list_pop_back(FreeList* list) {
    HeaderBlock* tail = list->tail;
    list->size -= get_size(tail);
    if (list->tail == list->head) {
        list->head = NULL;
        list->tail = NULL;
        return;
    }

    list->tail->prev->next = NULL;
    list->tail = list->tail->prev;
}

void list_remove(FreeList* list, HeaderBlock* block) {
    if (block == list->head)  {
        list_pop_front(list);
        return;
    }

    if (block == list->tail) {
        list_pop_back(list);
        return;
    }

    block->prev->next = block->next;
    block->next->prev = block->prev;
}

static HeaderBlock* list_find_prev(FreeList* list, void* block) {
    void* curr = (void*)list->head;
    if ((void*)list->tail < block) {
        // printf("[FREE] Tail: %p is smaller than block %p\n", (void*)list->tail, block);
        return ((HeaderBlock*) list->tail);
    }

    while (curr != NULL) {
        if (curr > block)
            return ((HeaderBlock*) curr)->prev;

        curr = (void*) ((HeaderBlock*) curr)->next;
    }
    
    return NULL;
}

static void list_coalesce(FreeList* list, HeaderBlock* block, HeaderBlock* prev, HeaderBlock* next) {
    size_t coalesceState = 0;
    coalesceState |= (size_t)(prev && (get_end_addr_block(prev) == (void*) block));
    coalesceState <<= 1;
    coalesceState |= (size_t)(next && (get_end_addr_block(block) == (void*) next));
    // printf("[FREE] Coalesce state: %lu\n", coalesceState);

    switch (coalesceState) {
        case 0:
            make_block(block, get_size(block), true, next, prev);
            if (prev)
                prev->next = block;
            else 
                list->head = block;
            
            if (next)
                next->prev = block;
            else 
                list->tail = block;

            return;

        case 1: {
            size_t nextSize = 0;
            HeaderBlock* newNext = NULL;
            if (next) {
                nextSize = get_size(next);
                newNext = next->next;
            }

            make_block(block, get_size(block) + nextSize, true, newNext, prev);
            if (prev)
                prev->next = block;
            else 
                list->head = block;

            if (newNext)
                newNext->prev = block;
            else
                list->tail = block;

            return;
        }

        case 2:
            make_block(prev, get_size(prev) + get_size(block), true, next, prev->prev);
            if (next)
                next->prev = prev;
            else 
                list->tail = prev;

            return;

        case 3:
            make_block(prev, get_size(prev) + get_size(block) + get_size(next), true, next->next, prev->prev);
            if (next->next)
                next->next->prev = prev;
            else
                list->tail = prev;

            return;

        default:
            printf("[ERROR] Coalesce has invalid state: %lu\n", coalesceState);
            exit(1);
    }

}

void list_insert_and_coalesce(FreeList* list, HeaderBlock* block) {
    if (list->head == NULL) {
        list_init(list, block, get_size(block));
        return;
    }

    list->size += get_size(block);

    HeaderBlock* prev = list_find_prev(list, block);
    if (prev == NULL) {
        // printf("[FREE] Trying to add %p to front of list starting %p\n", (void*) block, (void*) list->head);
        list_coalesce(list, block, NULL, list->head);
        list->head = block;
        return;
    }

    list_coalesce(list, block, prev, prev->next);
}

void list_print(FreeList* list) {
    HeaderBlock* curr = list->head;
    printf("--- Free Blocks State ---\n");
    while (curr != NULL) {
        print_block(curr);
        curr = curr->next;
    }
}