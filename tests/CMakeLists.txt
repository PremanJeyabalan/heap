FetchContent_Declare(catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
FetchContent_MakeAvailable(catch)

add_executable(testHeapBlock HeapBlockTests.cpp)
target_link_libraries(testHeapBlock PRIVATE
    heep vmem Catch2::Catch2 Catch2::Catch2WithMain fmt::fmt)
