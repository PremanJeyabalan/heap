add_executable(vmem_test
        vmem_test.c)
target_link_libraries(vmem_test
        PUBLIC vmem)
add_test(NAME vmem_test
        COMMAND $<TARGET_FILE:vmem_test>)

add_executable(hblock_test
        hblock_test.c)
target_link_libraries(hblock_test
        PUBLIC heep)
add_test(NAME hblock_test
        COMMAND $<TARGET_FILE:hblock_test>)

add_executable(flist_test
        flist_test.c)
target_link_libraries(flist_test
        PUBLIC heep)
add_test(NAME flist_test
        COMMAND $<TARGET_FILE:flist_test>)
