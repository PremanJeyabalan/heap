add_executable(EqualSizeFFBenchmark
        equal_size_benchmark.c)
target_link_libraries(EqualSizeFFBenchmark PRIVATE
        FirstFitHeap)
target_compile_definitions(EqualSizeFFBenchmark PRIVATE _GNU_SOURCE)

add_executable(EqualSizeBFBenchmark
        equal_size_benchmark.c)
target_link_libraries(EqualSizeBFBenchmark PRIVATE
        BestFitHeap)
target_compile_definitions(EqualSizeBFBenchmark PRIVATE _GNU_SOURCE)
