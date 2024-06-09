//
// Created by prema on 9/6/2024.
//

#include <heep/HeapManager.hpp>


int main() {
    struct F{};
    heep::HeapManager<F> heapManager{64};
    heapManager.print();
    return 0;
}