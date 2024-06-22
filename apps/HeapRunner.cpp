//
// Created by prema on 9/6/2024.
//

#include <heep/HeapManager.hpp>
#include <heep/Finders.hpp>


int main() {
    heep::HeapManager heapManager{64};
    heapManager.print();
    heapManager.allocate<heep::finders::FirstFit>(32);
    heapManager.print();
    return 0;
}