//
// Created by prema on 9/6/2024.
//

#include <catch2/catch_test_macros.hpp>
#include <heep/HeapBlock.hpp>
#include <heep/Helpers.hpp>
#include <vmem/VirtualStaticHeap.hpp>


TEST_CASE("Heap header blocks should inserted correctly", "[hblock]") {
    heep::vmem::VirtualStaticHeap heap;
    void* start = heap.request(128);

    SECTION("init should set header and footer correctly") {
        auto* block = static_cast<heep::HeapBlock*>(start);
        block->init(64, nullptr, nullptr);
        REQUIRE(block->getSize() == 64);
        REQUIRE(!block->getFree());
        void* expectedEndAddress = static_cast<char*>(start) + 64;
        REQUIRE(block->getEndAddr() == expectedEndAddress);
        REQUIRE(block->getFooterAddr() == (void*)(static_cast<char*>(expectedEndAddress) - heep::helpers::footer_size));
        auto* footer =  block->getFooterAddr();
        REQUIRE(footer->getSize() == block->getSize());
        REQUIRE(footer->getFree() == block->getFree());
        REQUIRE(footer->getNext() == block->getNext());
        REQUIRE(footer->getPrev() == block->getPrev());

        block->init(65, block, nullptr);
        REQUIRE(block->getSize() == 64);
        REQUIRE(block->getFree());
        expectedEndAddress = static_cast<char*>(start) + 64;
        REQUIRE(block->getEndAddr() == expectedEndAddress);
        REQUIRE(block->getFooterAddr() == (void*)(static_cast<char*>(expectedEndAddress) - heep::helpers::footer_size));
        REQUIRE(block->getNext() == block);
        footer =  block->getFooterAddr();
        REQUIRE(footer->getSize() == block->getSize());
        REQUIRE(footer->getFree() == block->getFree());
        REQUIRE(footer->getNext() == block->getNext());
        REQUIRE(footer->getPrev() == block->getPrev());
    }

    SECTION("expand should increase by increment only") {
        auto* block = static_cast<heep::HeapBlock*>(start);
        block->init(64, nullptr, nullptr);
        block->expand(16);
        REQUIRE(block->getSize() == 80);
        REQUIRE(!block->getFree());
        void* expectedEndAddress = static_cast<char*>(start) + 80;
        REQUIRE(block->getEndAddr() == expectedEndAddress);
        REQUIRE(block->getFooterAddr() == (void*)(static_cast<char*>(expectedEndAddress) - heep::helpers::footer_size));
        auto* footer =  block->getFooterAddr();
        REQUIRE(footer->getSize() == block->getSize());
        REQUIRE(footer->getFree() == block->getFree());
        REQUIRE(footer->getNext() == block->getNext());
        REQUIRE(footer->getPrev() == block->getPrev());

        block->init(65, nullptr, nullptr);
        block->expand(16);
        REQUIRE(block->getSize() == 80);
        REQUIRE(block->getFree());
        expectedEndAddress = static_cast<char*>(start) + 80;
        REQUIRE(block->getEndAddr() == expectedEndAddress);
        REQUIRE(block->getFooterAddr() == (void*)(static_cast<char*>(expectedEndAddress) - heep::helpers::footer_size));
        footer =  block->getFooterAddr();
        REQUIRE(footer->getSize() == block->getSize());
        REQUIRE(footer->getFree() == block->getFree());
        REQUIRE(footer->getNext() == block->getNext());
        REQUIRE(footer->getPrev() == block->getPrev());
    }
}