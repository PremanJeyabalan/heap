set(BENCHMARK_ENABLE_TESTING off)
FetchContent_Declare(gbench
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main)
FetchContent_MakeAvailable(gbench)

add_executable(equalSizeBenchmark EqualSizeBenchmark.cpp)
target_link_libraries(equalSizeBenchmark PRIVATE
    heep vmem benchmark::benchmark benchmark::benchmark_main fmt::fmt)
