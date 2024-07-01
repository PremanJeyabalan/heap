//
// Created by prema on 1/7/2024.
//
#include <benchmark/benchmark.h>
#include <heep/HeapManager.hpp>
#include <heep/Finders.hpp>

//static void BM_StringCreation(benchmark::State& state) {
//    for (auto _ : state)
//        std::string empty_string;
//}
//
//BENCHMARK(BM_StringCreation);
//
//static void BM_StringCopy(benchmark::State& state) {
//    std::string x = "hello";
//    for (auto _ : state)
//        std::string copy(x);
//}
//
//BENCHMARK(BM_StringCopy);

constexpr int num_items = 10'000;

static void BM_EqualAlloc_BF(benchmark::State& state) {
    int* array[num_items]{};
    int* spacing_array[num_items]{};

    for (auto _ : state) {
        state.PauseTiming();
        heep::HeapManager hm{heep::helpers::page_size};

        for (int i = 0; i < num_items; i++) {
            array[i] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(state.range(0)));
            spacing_array[i] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(state.range(0)));
        }

        for (int i = 0; i < num_items; i++)
            hm.deallocate(array[i]);

        state.ResumeTiming();

        for (int i = 0; i < num_items; i++) {
            for (int j = 0; j < 1000; j++) {
                array[j] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(state.range(0)));
            }

            for (int j = 1000; j < num_items; j++) {
                array[j] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(state.range(0)));
                hm.deallocate(array[j-1000]);

            }

            for (int j = num_items - 1000; j < num_items; j++)
                hm.deallocate(array[j]);
        }
    }
}

BENCHMARK(BM_EqualAlloc_BF)->Range(128, 2 << 10);