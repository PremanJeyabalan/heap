//
// Created by prema on 2/5/2024.
//
//#define _GNU_SOURCE
#include <time.h>

#include "hbench/hbench.h"
#include "heep/heap.h"

#define NUM_ITERS    10000
#define NUM_ITEMS    10000
#define ALLOC_SIZE   128

#define MALLOC(x) heap_malloc(x)
#define FREE(x) heap_free(x)


double calc_time(struct timespec start, struct timespec end) {
    double start_sec = (double)start.tv_sec * 1000000000.0 + (double)start.tv_nsec;
    double end_sec = (double)end.tv_sec * 1000000000.0 + (double)end.tv_nsec;

    if (end_sec < start_sec) {
        return 0;
    }
    else {
        return end_sec - start_sec;
    }
}

int main(void) {
    int i, j;
    int* array[NUM_ITEMS];
    int* spacing_array[NUM_ITEMS];
    unsigned long data_segment_size;
    unsigned long data_segment_free_space;
    struct timespec start_time, end_time;
//    heap_init(GET_ALIGNED_BLOCK(ALLOC_SIZE) * NUM_ITEMS * 4);
    heap_init(GET_ALIGNED_BLOCK(ALLOC_SIZE)*NUM_ITEMS);

    for (i = 0; i < NUM_ITEMS; i++) {
        array[i] = (int *)MALLOC(ALLOC_SIZE);
        spacing_array[i] = (int *)MALLOC(ALLOC_SIZE);
    }  //for i

    for (i = 0; i < NUM_ITEMS; i++) {
        FREE(array[i]);
    }  //for i

    //Start Time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (i = 0; i < NUM_ITERS; i++) {
        for (j = 0; j < 1000; j++) {
            array[j] = (int *)MALLOC(ALLOC_SIZE);
        }  //for j

        for (j = 1000; j < NUM_ITEMS; j++) {
            array[j] = (int *)MALLOC(ALLOC_SIZE);
            FREE(array[j - 1000]);

            if ((i == NUM_ITERS / 2) && (j == NUM_ITEMS / 2)) {
                //Record fragmentation halfway through (try to repsresent steady state)
                data_segment_size = heap_get_size();
                data_segment_free_space = heap_get_free_size();
            }  //if
        }    //for j

        for (j = NUM_ITEMS - 1000; j < NUM_ITEMS; j++) {
            FREE(array[j]);
        }  //for j
    }    //for i

    //Stop Time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed_ns = calc_time(start_time, end_time);
    printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
    printf("Fragmentation  = %f\n",
           (float)data_segment_free_space / (float)data_segment_size);

//    for (i = 0; i < NUM_ITEMS; i++) {
//        FREE(spacing_array[i]);
//    }

    heap_collect();
}
