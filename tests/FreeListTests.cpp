//
// Created by prema on 22/6/2024.
//

#include <catch2/catch_test_macros.hpp>
#include <heep/FreeList.hpp>
#include <heep/Helpers.hpp>


TEST_CASE("Free List should correctly manage linked list of free blocks", "[freelist]") {
    char vHeap[heep::helpers::page_size*10];
    heep::FreeList fList{vHeap, heep::helpers::page_size*10};

    SECTION("Push back to an empty free list should create one element only") {
        fList = heep::FreeList();
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);

        fList.pushBack(vHeap, 64);
        auto* head = static_cast<heep::HeapBlock*>(fList.getHead());
        REQUIRE(fList.size() == 64);
        REQUIRE(static_cast<void*>(head) == static_cast<void*>(vHeap));
        REQUIRE(head->getFree());
        REQUIRE(head->getSize() == 64);
        REQUIRE(head->getPrev() == nullptr);
        REQUIRE(head->getNext() == nullptr);
        REQUIRE(head == fList.getTail());
    }
}