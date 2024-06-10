//
// Created by prema on 10/6/2024.
//
#include <catch2/catch_test_macros.hpp>
#include <heep/HeapBlock.hpp>
#include <heep/Helpers.hpp>
#include <vmem/VirtualStaticHeap.hpp>



using namespace heep::helpers;
TEST_CASE("Helpers should perform correct bit manipulation", "[helpers]") {

    SECTION("Get Aligned 16 should round up to the nearest 16 byte alignment") {
        REQUIRE(get_aligned_16_bytes(1) == 16);
        REQUIRE(get_aligned_16_bytes(15) == 16);
        REQUIRE(get_aligned_16_bytes(16) == 16);
        REQUIRE(get_aligned_16_bytes(17) == 32);
    }

    SECTION("Get Aligned Block should create a valid sized 16-byte aligned block") {
        REQUIRE(get_aligned_block(1) == 64);
        REQUIRE(get_aligned_block(15) == 64);
        REQUIRE(get_aligned_block(16) == 64);
        REQUIRE(get_aligned_block(17) == 80);
    }

    SECTION("Get Size From Pack should correctly read block size from size free pack") {
        REQUIRE(get_size_from_pack(1) == 0);
        REQUIRE(get_size_from_pack(15) == 0);
        REQUIRE(get_size_from_pack(16) == 16);
        REQUIRE(get_size_from_pack(31) == 16);
        REQUIRE(get_size_from_pack(32) == 32);
    }

    SECTION("Get Free From Pack should correctly read block free from size free pack") {
        REQUIRE(get_free_from_pack(1));
        REQUIRE(get_free_from_pack(17));
        REQUIRE(!get_free_from_pack(32));
        REQUIRE(get_free_from_pack(33));
    }

    SECTION("Size And Free To Pack should pack together size and free") {
        REQUIRE(size_and_free_to_pack(0, 1) == 1);
        REQUIRE(size_and_free_to_pack(16, 0) == 16);
        REQUIRE(size_and_free_to_pack(16, 1) == 17);
        REQUIRE(size_and_free_to_pack(32, 0) == 32);
        REQUIRE(size_and_free_to_pack(32, 1) == 33);
    }

    SECTION("Get End Address should give correct end address from pointer") {
        char vheap[128]{};
        void* start = (void*) vheap;
        REQUIRE((char*) get_block_end_address_from_start(start, 64) == vheap+64);
        REQUIRE((char*) get_block_end_address_from_start(start, 16) == vheap+16);
        REQUIRE((char*) get_block_end_address_from_start(start, 0) == vheap);
    }

    SECTION("Get Footer Address should give corect footer address") {
        char vheap[128]{};
        void* start = (void*) vheap;
        REQUIRE((char*) get_block_footer_address_from_start(start, 64) == vheap+32);
        REQUIRE((char*) get_block_footer_address_from_start(start, 80) == vheap+48);
        REQUIRE((char*) get_block_footer_address_from_start(start, 96) == vheap+64);
    }

    SECTION("Get Start Address From Data should give correct start address") {
        char vheap[128]{};
        void* start = (void*) vheap;
        void* data = (void*)(vheap+alloc_header_size);
        REQUIRE(get_block_start_address_from_data(data) == start);

        void* start2 = (void*)(vheap+64);
        void* data2 = (void*)(vheap+80);
        REQUIRE(get_block_start_address_from_data(data2)==start2);
    }

    SECTION("Get Start Address From Footer should give correct start address") {
        char vheap[128]{};
        void* start = (void*) vheap;
        void* footer = get_block_footer_address_from_start(start, 64);
        REQUIRE(get_block_start_address_from_footer(footer, 64) == start);

        void* start2 = (void*)(vheap+64);
        void* footer2 = get_block_footer_address_from_start(start2, 64);
        REQUIRE(get_block_start_address_from_footer(footer2, 64) == start2);
    }

    SECTION("Get Previous Footer Address should give correct footer address") {
        char vheap[192]{};
        void* start = (void*) vheap;
        void* footer = get_block_footer_address_from_start(start, 64);
        void* start2 = get_block_end_address_from_start(start, 64);
        REQUIRE(get_block_prev_footer_address(start2) == footer);

        void* start3 = get_block_end_address_from_start(start2, 48);
        REQUIRE(get_block_prev_footer_address(start3) == get_block_footer_address_from_start(start2, 48));
    }

    SECTION("Get Previous Start Address should give correct start address") {
        char vheap[192]{};
        void* start = (void*) vheap;
        void* start2 = get_block_end_address_from_start(start, 64);
        void* footer = get_block_prev_footer_address(start2);
        auto* fHBlock = static_cast<heep::HeapBlock*>(footer);
        fHBlock->init(64, static_cast<heep::HeapBlock*>(start2), nullptr);
        REQUIRE(get_block_prev_start_address(start2) == start);

        void* start3 = get_block_end_address_from_start(start2, 48);
        void* footer2 = get_block_footer_address_from_start(start2, 48);
        auto* fHBlock2 = static_cast<heep::HeapBlock*>(footer2);
        fHBlock2->init(49, nullptr, static_cast<heep::HeapBlock*>(start2));

        REQUIRE(get_block_prev_start_address(start3) == start2);
    }
}
