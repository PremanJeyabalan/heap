//
// Created by prema on 23/6/2024.
//
#include <catch2/catch_test_macros.hpp>

#include <heep/HeapManager.hpp>
#include <heep/Helpers.hpp>
#include <heep/Finders.hpp>

TEST_CASE("Heap Manager should correctly expand heap by incremental aligned bytes", "[heap_manager]") {
    using namespace heep::helpers;
    heep::HeapManager hm{1*page_size};


    SECTION("Expanding fully allocated heap should not coalesce") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(253*16);
        REQUIRE(hm.size() == 1*page_size);
        REQUIRE(hm.capacity() == 1*page_size);
        REQUIRE(hm.getFreeSize() == 0);

        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1);
        REQUIRE(hm.capacity() == 2*page_size);
        REQUIRE(hm.size() == page_size + 64);
    }

    SECTION("Expanding heap where last block is alloced should not coalesce") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(2001);
        REQUIRE(hm.size() == 2064);
        REQUIRE(hm.capacity() == page_size);

        auto* startFirstAlloced = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr));
        auto* firstFreeBlock = static_cast<heep::HeapBlock*>(get_block_prev_start_address(startFirstAlloced));
        REQUIRE(firstFreeBlock->getFree());
        REQUIRE(firstFreeBlock->getSize() == 2032);

        auto* ptr2 = hm.allocate<heep::finders::BestFit>(2000);
        auto* secondFreeBlock = static_cast<heep::HeapBlock *>(startFirstAlloced->getEndAddr());
        REQUIRE(firstFreeBlock->getNext() == secondFreeBlock);
        REQUIRE(secondFreeBlock->getPrev() == firstFreeBlock);
    }
    SECTION("Expanding heap where last block is free should coalesce") {}
}