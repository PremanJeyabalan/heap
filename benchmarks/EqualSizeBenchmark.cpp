//
// Created by prema on 1/7/2024.
//
#include <benchmark/benchmark.h>
#include <heep/HeapManager.hpp>
#include <heep/Finders.hpp>
#include <fmt/format.h>

constexpr int num_items = 1000;
constexpr int diff = num_items / 10;

static void BM_EqualAlloc_BF(benchmark::State& state) {
    int* array[num_items]{};
    int* spacing_array[num_items]{};
    auto alloc_size = 1 << state.range(0);

    heep::HeapManager hm{heep::helpers::page_size};

    for (int i = 0; i < num_items; i++) {
        array[i] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(alloc_size));
        spacing_array[i] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(alloc_size));
    }

    for (int i = 0; i < num_items; i++) {
        hm.deallocate(array[i]);
    }

    while (state.KeepRunning()) {
        for (int i = 0; i < num_items; i++) {
            for (int j = 0; j < diff; j++) {
                benchmark::DoNotOptimize(array[j] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(alloc_size)));
            }

            for (int j = diff; j < num_items; j++) {
                benchmark::DoNotOptimize(array[j] = static_cast<int*>(hm.allocate<heep::finders::BestFit>(alloc_size)));
                hm.deallocate(array[j-diff]);
            }

            for (int j = num_items - diff; j < num_items; j++)
                hm.deallocate(array[j]);
        }
    }

    state.SetBytesProcessed(long{state.iterations()} * long{alloc_size});
    state.SetLabel(std::to_string(alloc_size / 1024) + "kb");
}

BENCHMARK(BM_EqualAlloc_BF)
    ->DenseRange(13,26)
    ->Unit(benchmark::kMicrosecond)
    ->ReportAggregatesOnly(true);