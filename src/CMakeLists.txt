set(HEADER_LIST_VMEM
        ${PROJECT_SOURCE_DIR}/include/vmem/vmem.h)

set(HEADER_LIST_HEEP
        ${PROJECT_SOURCE_DIR}/include/heep/hblock.h
        ${PROJECT_SOURCE_DIR}/include/heep/flist.h
        ${PROJECT_SOURCE_DIR}/include/heep/heap.h)


add_library(vmem
        vmem/vmem.c
        ${HEADER_LIST_VMEM})

target_include_directories(vmem PUBLIC ${PROJECT_SOURCE_DIR}/include/vmem)

set(HEADER_LIST
        ${HEADER_LIST_VMEM}
        ${HEADER_LIST_HEEP})

add_library(heep
        heep/hblock.c
        heep/flist.c
        heep/heap.c
        ${HEADER_LIST_HEEP})

target_include_directories(heep PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(heep PRIVATE vmem)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)
