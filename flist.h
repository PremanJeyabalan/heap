
#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "hblock.h"

typedef struct {
    HeaderBlock* head;
    HeaderBlock* tail;
    size_t size;
} FreeList;

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

void list_print(FreeList* list) {
    HeaderBlock* curr = list->head;
    printf("--- Free Blocks State ---\n");
    while (curr != NULL) {
        print_block(curr);
        curr = curr->next;
    }
}

#endif