#ifndef MEM_H
#define MEM_H


#include <stdlib.h>

void mem_init();
void* mem_sbrk(size_t increment);

#endif