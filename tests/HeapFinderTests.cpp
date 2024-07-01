//
// Created by prema on 1/7/2024.
//
#include <catch2/catch_test_macros.hpp>

#include <heep/HeapManager.hpp>
#include <heep/Helpers.hpp>
#include <heep/FreeList.hpp>
#include <heep/HeapBlock.hpp>
#include <heep/Finders.hpp>

TEST_CASE("Best Fit should correctly find victim free block", "[finders]") {
    using namespace heep::helpers;
    char vHeap[page_size]{};
    heep::FreeList fList{};
    heep::finders::BestFit bf{};

    SECTION("When fully alloced should return empty victim") {
        auto victim = bf(fList, 1);
        REQUIRE(!victim.has_value());
    }

    SECTION("When not enough space should return empty victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 32);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 48), 48);

        auto victim = bf(fList, 96);
        REQUIRE(!victim.has_value());
    }

    SECTION("When one valid free space should return that as victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 128);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128), 48);

        auto victim = bf(fList, 96);
        REQUIRE(victim.has_value());
        REQUIRE(victim.value() == ptr2);
    }

    SECTION("When multiple free space and one exact should return that as victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 128);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128), 96);

        auto victim = bf(fList, 96);
        REQUIRE(victim.has_value());
        REQUIRE(victim.value() == ptr3);
    }

    SECTION("When multiple free space and none exact should return closest valid") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 128);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128), 64);
        auto* ptr4 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128+64), 48);

        auto victim = bf(fList, 32);
        REQUIRE(victim.has_value());
        REQUIRE(victim.value() == ptr4);
    }
}

TEST_CASE("First Fit should correctly find victim free block", "[finders]") {
    using namespace heep::helpers;
    char vHeap[page_size]{};
    heep::FreeList fList{};
    heep::finders::FirstFit ff{};

    SECTION("When fully alloced should return empty victim") {
        auto victim = ff(fList, 1);
        REQUIRE(!victim.has_value());
    }

    SECTION("When not enough space should return empty victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 32);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 48), 48);

        auto victim = ff(fList, 96);
        REQUIRE(!victim.has_value());
    }

    SECTION("When one valid free space should return that as victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 128);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128), 48);

        auto victim = ff(fList, 96);
        REQUIRE(victim.has_value());
        REQUIRE(victim.value() == ptr2);
    }

    SECTION("When multiple free space should return first as victim") {
        auto* ptr = fList.pushBack(vHeap, 16);
        auto* ptr2 = fList.pushBack(get_block_end_address_from_start(vHeap, 16), 128);
        auto* ptr3 = fList.pushBack(get_block_end_address_from_start(vHeap, 16+128), 96);

        auto victim = ff(fList, 96);
        REQUIRE(victim.has_value());
        REQUIRE(victim.value() == ptr2);
    }
}
