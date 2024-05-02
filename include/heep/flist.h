
#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "hblock.h"

typedef struct {
    HBlock* head;
    HBlock* tail;
    size_t size;
} FreeList;

void list_init(FreeList* list, void* memory, size_t size);
HBlock* list_append(FreeList* list, void* memory, size_t size);
void list_pop_front(FreeList* list);
void list_pop_back(FreeList* list);
void list_remove(FreeList* list, HBlock* block);
void list_insert_and_coalesce(FreeList* list, HBlock* block);
void list_print(FreeList* list);

#endif