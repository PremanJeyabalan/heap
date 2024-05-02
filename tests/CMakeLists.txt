add_executable(vmem_test
        vmem_test.c)
target_link_libraries(vmem_test
        PUBLIC vmem)
add_test(NAME vmem_test
        COMMAND $<TARGET_FILE:vmem_test>)

add_executable(hblock_test
        hblock_test.c)
target_link_libraries(hblock_test
        PUBLIC FirstFitHeap)
add_test(NAME hblock_test
        COMMAND $<TARGET_FILE:hblock_test>)

add_executable(flist_test
        flist_test.c)
target_link_libraries(flist_test
        PUBLIC FirstFitHeap)
add_test(NAME flist_test
        COMMAND $<TARGET_FILE:flist_test>)

add_executable(heap_test
        heap_test.c)
target_link_libraries(heap_test
        PUBLIC FirstFitHeap)
add_test(NAME heap_test
        COMMAND $<TARGET_FILE:heap_test>)

add_executable(heap_malloc_ff_data_test
        heap_malloc_data_test.c)
target_link_libraries(heap_malloc_ff_data_test
        PRIVATE FirstFitHeap)
add_test(NAME heap_malloc_ff_data_test
        COMMAND $<TARGET_FILE:heap_malloc_ff_data_test>)


add_executable(heap_malloc_bf_data_test
        heap_malloc_data_test.c)
target_link_libraries(heap_malloc_bf_data_test
        PRIVATE BestFitHeap)
add_test(NAME heap_malloc_bf_data_test
        COMMAND $<TARGET_FILE:heap_malloc_bf_data_test>)
