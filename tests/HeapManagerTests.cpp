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

    SECTION("Expanding fully alloced heap should not coalesce") {
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

    SECTION("Expanding heap where last block is free should coalesce") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(100);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(100);
        hm.deallocate(ptr);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(page_size);

        REQUIRE(hm.capacity() == 3*page_size);
        REQUIRE(hm.size() == (get_aligned_block(page_size) + get_aligned_block(100)));

        auto* firstFreeBlock = static_cast<heep::HeapBlock*>(get_block_prev_start_address(
                get_block_start_address_from_data(ptr2)));
        auto* firstAllocBlock = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr2));
        auto* secondFreeBlock = static_cast<heep::HeapBlock*>(firstAllocBlock->getEndAddr());

        REQUIRE(firstFreeBlock->getNext() == secondFreeBlock);
        REQUIRE(secondFreeBlock->getPrev() == firstFreeBlock);
        REQUIRE(secondFreeBlock->getNext() == nullptr);
    }
}

TEST_CASE("Heap Manager should correctly coalesce on deallocate", "[heap_manager]") {
    using namespace heep::helpers;
    heep::HeapManager hm{1*page_size};

    SECTION("Deallocating surrounded alloc and empty free list should not coalesce") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(1328);
        REQUIRE(hm.getFreeSize() == 0);
//        hm.print();

        auto* ptr2Block = static_cast<heep::HeapBlock *>(get_block_start_address_from_data(ptr2));
        size_t ptr2Size = ptr2Block->getSize();
        hm.deallocate(ptr2);

        REQUIRE(hm.getFreeSize() == ptr2Size);
        REQUIRE(ptr2Block->getFree());
        REQUIRE(ptr2Block->getPrev() == nullptr);
        REQUIRE(ptr2Block->getNext() == nullptr);
        REQUIRE(hm.getFreeHead() == ptr2Block);
        REQUIRE(hm.getFreeTail() == ptr2Block);
    }

    SECTION("Deallocating surrounded alloc and non-empty free list should not coalesce") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(1328);
        auto* ptr4 = hm.allocate<heep::finders::BestFit>(1);


        auto* ptr2Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr2));
        auto* ptrBlock = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr));
        auto* firstFreeBlock = static_cast<heep::HeapBlock*>(ptrBlock->getEndAddr());
        size_t ptr2Size = ptr2Block->getSize();
        hm.deallocate(ptr2);

        REQUIRE(ptr2Block->getFree());
        REQUIRE(ptr2Block->getPrev() == nullptr);
        REQUIRE(ptr2Block->getNext() == firstFreeBlock);
        REQUIRE(hm.getFreeHead() == ptr2Block);
        REQUIRE(hm.getFreeTail() == ptrBlock->getEndAddr());
        REQUIRE(firstFreeBlock->getPrev() == ptr2Block);
    }

    SECTION("Deallocating alloc with right free coalesce right only") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(1328);


        auto* ptrBlock = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr));
        auto* ptr2Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr2));
        size_t expectedSize = ptrBlock->getSize() + ptr2Block->getSize();

        hm.deallocate(ptr);
        hm.deallocate(ptr2);

        REQUIRE(ptr2Block->getSize() == expectedSize);
        REQUIRE(ptr2Block->getFree());
        REQUIRE(ptr2Block->getPrev() == nullptr);
        REQUIRE(ptr2Block->getNext() == nullptr);
        REQUIRE(hm.getFreeHead() == ptr2Block);
        REQUIRE(hm.getFreeTail() == ptr2Block);
        REQUIRE(hm.getFreeSize() == expectedSize);
    }

    SECTION("Deallocating alloc with left free coalesce left only") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(1328);


        auto* ptr3Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr3));
        auto* ptr2Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr2));
        size_t expectedSize = ptr3Block->getSize() + ptr2Block->getSize();

        hm.deallocate(ptr3);
        hm.deallocate(ptr2);

        REQUIRE(ptr3Block->getSize() == expectedSize);
        REQUIRE(ptr3Block->getFree());
        REQUIRE(ptr3Block->getPrev() == nullptr);
        REQUIRE(ptr3Block->getNext() == nullptr);
        REQUIRE(hm.getFreeHead() == ptr3Block);
        REQUIRE(hm.getFreeTail() == ptr3Block);
        REQUIRE(hm.getFreeSize() == expectedSize);
    }

    SECTION("Deallocating alloc with both free coalesce both") {
        auto* ptr = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr2 = hm.allocate<heep::finders::BestFit>(1300);
        auto* ptr3 = hm.allocate<heep::finders::BestFit>(1328);


        auto* ptr3Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr3));
        auto* ptr2Block = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr2));
        auto* ptrBlock = static_cast<heep::HeapBlock*>(get_block_start_address_from_data(ptr));
        size_t expectedSize = ptr3Block->getSize() + ptr2Block->getSize() + ptrBlock->getSize();


        hm.deallocate(ptr);
        hm.deallocate(ptr3);
        hm.deallocate(ptr2);

        REQUIRE(ptr3Block->getSize() == expectedSize);
        REQUIRE(ptr3Block->getFree());
        REQUIRE(ptr3Block->getPrev() == nullptr);
        REQUIRE(ptr3Block->getNext() == nullptr);
        REQUIRE(hm.getFreeHead() == ptr3Block);
        REQUIRE(hm.getFreeTail() == ptr3Block);
        REQUIRE(hm.getFreeSize() == expectedSize);
    }
}

TEST_CASE("Heap manager should correctly store pointers", "[heap_manager]") {
    using namespace heep::helpers;
    heep::HeapManager hm{1*page_size};

    auto allocTest = [&]<typename Finder>(Finder f) -> std::pair<size_t, size_t> {
        size_t runningSum{};
        size_t expectedSum{};
        int* array[10]{};
        int sizes1[] = {4, 16, 8, 32, 7, 256, 23, 4, 10, 32};

        auto allocSizeArray = [&](int size, int** arr, int index){
            expectedSum += size * size;
            arr[index] = static_cast<int*>(hm.allocate<Finder>(size * sizeof(int)));
            for (int i = 0; i < size; i++) {
                arr[index][i] = size;
            }
            for (int i = 0; i < size; i++) {
                runningSum += arr[index][i];
            }
        };

        for (int i = 0; i < 4; i++)
            allocSizeArray(sizes1[i], array, i);

        hm.deallocate(array[0]);
        hm.deallocate(array[2]);

        for (int i = 4; i < 6; i++)
            allocSizeArray(sizes1[i], array, i);

        hm.deallocate(array[5]);
        hm.deallocate(array[1]);
        hm.deallocate(array[3]);


        for (int i = 6; i < 8; i++)
            allocSizeArray(sizes1[i], array, i);

        hm.deallocate(array[4]);

        for (int i = 8; i < 10; i++)
            allocSizeArray(sizes1[i], array, i);

        hm.deallocate(array[6]);
        hm.deallocate(array[7]);
        hm.deallocate(array[8]);
        hm.deallocate(array[9]);

        return std::make_pair(runningSum, expectedSum);
    };

    SECTION("Different sized allocs should not corrupt stored data with best fit") {
        auto [calcBFSum, expectedBFSum] = allocTest(heep::finders::BestFit{});
        REQUIRE(calcBFSum == expectedBFSum);
    }

    SECTION("Different sized allocs should not corrupt stored data with first fit") {
        auto [calcFFSum, expectedFFSum] = allocTest(heep::finders::FirstFit{});
        REQUIRE(calcFFSum == expectedFFSum);
    }
}