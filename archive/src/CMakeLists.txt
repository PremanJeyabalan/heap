set(HEADER_LIST_VMEM
        ${PROJECT_SOURCE_DIR}/include/vmem/vmem.h)

set(HEADER_LIST_HEEP
        ${PROJECT_SOURCE_DIR}/include/heep/hblock.h
        ${PROJECT_SOURCE_DIR}/include/heep/flist.h
        ${PROJECT_SOURCE_DIR}/include/heep/heap.h)

set(HEADER_LIST_HEEP_BENCHMARK
        ${PROJECT_SOURCE_DIR}/include/hbench/hbench.h)

set(HEADER_LIST
        ${HEADER_LIST_VMEM}
        ${HEADER_LIST_HEEP}
        ${HEADER_LIS_HEEP_BENCHMARK})

add_library(vmem
        vmem/vmem.c
        ${HEADER_LIST_VMEM})
target_include_directories(vmem PUBLIC ${PROJECT_SOURCE_DIR}/include/vmem)


add_library(first_fit_macro INTERFACE)
target_compile_definitions(first_fit_macro INTERFACE FIRST_FIT_FIND)
add_library(best_fit_macro INTERFACE)
target_compile_definitions(best_fit_macro INTERFACE BEST_FIT_FIND)


add_library(FirstFitHeap
        heep/hblock.c
        heep/flist.c
        heep/heap.c
        ${HEADER_LIST_HEEP})
target_include_directories(FirstFitHeap PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(FirstFitHeap
        PRIVATE vmem
        PUBLIC first_fit_macro)


add_library(BestFitHeap
        heep/hblock.c
        heep/flist.c
        heep/heap.c
        ${HEADER_LIST_HEEP})
target_include_directories(BestFitHeap PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(BestFitHeap
        PRIVATE vmem
        PUBLIC best_fit_macro)


add_library(HeepBenchmark
        hbench/hbench.c
        ${HEADER_LIST_HEEP_BENCHMARK})
target_include_directories(HeepBenchmark PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(HeepBenchmark
        PRIVATE FirstFitHeap)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)
