set(HEADER_LIST_VMEM
        ${PROJECT_SOURCE_DIR}/include/vmem/VirtualStaticHeap.hpp
        ../include/vmem/IVirtualStaticHeap.hpp)


set(HEADER_LIST_HEEP
        ${PROJECT_SOURCE_DIR}/include/heep/HeapBlock.hpp
        ${PROJECT_SOURCE_DIR}/include/heep/FreeList.hpp
        ${PROJECT_SOURCE_DIR}/include/heep/HeapManager.hpp
        ${PROJECT_SOURCE_DIR}/include/heep/Helpers.hpp
        ../include/heep/Finders.hpp
        ../include/heep/IHeapManager.hpp
        ../include/heep/IFreeList.hpp ../include/heep/IFinders.hpp)

set(HEADER_LIST
        ${HEADER_LIST_VMEM}
        ${HEADER_LIST_HEEP})

add_library(vmem
        vmem/VirtualStaticHeap.cpp
        ${HEADER_LIST_VMEM})
target_include_directories(vmem PUBLIC ${PROJECT_SOURCE_DIR}/include/vmem)

add_library(heep
        heep/HeapBlock.cpp heep/FreeList.cpp
        ${HEADER_LIST_HEEP} heep/HeapManager.cpp)
target_include_directories(heep PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(heep
        PRIVATE vmem
        PUBLIC fmt)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)
