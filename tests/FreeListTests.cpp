//
// Created by prema on 22/6/2024.
//

#include <catch2/catch_test_macros.hpp>
#include <heep/FreeList.hpp>
#include <heep/Helpers.hpp>


TEST_CASE("Free List should correctly manage linked list of free blocks", "[freelist]") {
    char vHeap[heep::helpers::page_size*10];
    heep::FreeList fList{vHeap, heep::helpers::page_size};

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

    SECTION("Push back to a non-empty list should increase it's size") {
        REQUIRE(fList.size() == heep::helpers::page_size);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* newTail = fList.getTail();
        REQUIRE(fList.size() == heep::helpers::page_size*2);

        REQUIRE(fList.getHead() == head);
        REQUIRE(head->getPrev() == nullptr);
        REQUIRE(head->getNext() == newTail);
        REQUIRE(head->getSize() == heep::helpers::page_size);

        REQUIRE(newTail == head->getEndAddr());
        REQUIRE(newTail->getPrev() == head);
        REQUIRE(newTail->getNext() == nullptr);
        REQUIRE(newTail->getSize() == heep::helpers::page_size);
    }

    SECTION("Pop back to an empty list should be no-op") {
        fList = heep::FreeList{};
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);

        fList.popBack();
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);
    }

    SECTION("Pop back to a single element list should make it empty") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        fList.popBack();
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);
    }

    SECTION("Pop back to a multiple element list should make it decrease nodes by one") {
        auto* head = fList.getHead();
        fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto size = fList.size();
        auto tailSize = fList.getTail()->getSize();
        size_t expectedSize = size - tailSize;

        fList.popBack();
        REQUIRE(fList.size() == expectedSize);
        REQUIRE(fList.getHead() == head);
        REQUIRE(head->getNext() == nullptr);
        REQUIRE(head->getPrev() == nullptr);
    }

    SECTION("Pop front on empty list should be no-op") {
        fList = heep::FreeList{};
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);

        fList.popFront();
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);
    }

    SECTION("Pop front on single element list should make it empty") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        fList.popFront();
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);

    }

    SECTION("Pop front on multiple element list reassign head") {
        auto* head = fList.getHead();
        fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto size = fList.size();
        auto tail = fList.getTail();
        auto tailSize = tail->getSize();
        size_t expectedSize = size - tailSize;

        fList.popFront();
        REQUIRE(fList.size() == expectedSize);
        REQUIRE(fList.getHead() == tail);
        REQUIRE(tail->getNext() == nullptr);
        REQUIRE(tail->getPrev() == nullptr);
    }

    SECTION("Erase on empty list should be no-op") {
        fList = heep::FreeList{};
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);

        void* tempPtr = static_cast<void*>(vHeap);
        fList.erase(static_cast<heep::HeapBlock*>(tempPtr));
        REQUIRE(fList.size() == 0);
        REQUIRE(fList.getHead() == nullptr);
        REQUIRE(fList.getTail() == nullptr);
    }

    SECTION("Erase on head of list should make it reassign head") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        auto* middle = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(fList.getTail()->getEndAddr(), heep::helpers::page_size);

        fList.erase(head);
        REQUIRE(fList.size() == 2*heep::helpers::page_size);
        REQUIRE(fList.getHead() == middle);
        REQUIRE(fList.getTail() == tail);

    }

    SECTION("Erase on tail of list reassigns tail") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        auto* middle = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(fList.getTail()->getEndAddr(), heep::helpers::page_size);

        fList.erase(tail);
        REQUIRE(fList.size() == 2*heep::helpers::page_size);
        REQUIRE(fList.getHead() == head);
        REQUIRE(fList.getTail() == middle);
    }

    SECTION("Erase on middle of list should reassign ptrs correctly") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        auto* middle = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(fList.getTail()->getEndAddr(), heep::helpers::page_size);

        fList.erase(middle);
        REQUIRE(fList.size() == 2*heep::helpers::page_size);
        REQUIRE(fList.getHead() == head);
        REQUIRE(fList.getTail() == tail);
        REQUIRE(head->getNext() == tail);
        REQUIRE(head->getPrev() == nullptr);
        REQUIRE(tail->getNext() == nullptr);
        REQUIRE(tail->getPrev() == head);
    }

    SECTION("Resize block to empty should remove it from the list") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        auto* middle = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(fList.getTail()->getEndAddr(), heep::helpers::page_size);

        auto newHead = fList.resizeBlock(head, 0);
        REQUIRE(!newHead.has_value());
        REQUIRE(fList.size() == 2*heep::helpers::page_size);
        REQUIRE(fList.getHead() == middle);
        REQUIRE(middle->getPrev() == nullptr);
        REQUIRE(fList.getTail() == tail);
    }

    SECTION("Resize block to smaller size should change list size") {
        REQUIRE(fList.size() > 0);
        REQUIRE(fList.getHead() != nullptr);
        REQUIRE(fList.getHead() == fList.getTail());

        auto* head = fList.getHead();
        auto* middle = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(fList.getTail()->getEndAddr(), heep::helpers::page_size);

        size_t newSize = 64;
        size_t expectedBlockSize = 64;
        size_t expectedListSize = fList.size() - (head->getSize() - newSize);

        auto newHead = fList.resizeBlock(head, newSize);
        REQUIRE(newHead.has_value());
        REQUIRE(newHead.value()->getSize() == expectedBlockSize);
        REQUIRE(fList.size() == expectedListSize);
        REQUIRE(fList.getHead() == newHead.value());
    }
}

TEST_CASE("Free List should correctly find neighbouring free blocks for a new block", "[freelist]") {
    char vHeap[heep::helpers::page_size*10];
    heep::FreeList fList{vHeap, heep::helpers::page_size};

    SECTION("An empty list should return both empty") {
        fList = heep::FreeList{};
        void* tempPtr = static_cast<void*>(vHeap);
        auto [prev, next] = fList.findPrevAndNextFree(static_cast<heep::HeapBlock*>(tempPtr));
        REQUIRE(prev == nullptr);
        REQUIRE(next == nullptr);
    }

    SECTION("When block has largest address it should be appended") {
        auto* tempPtr = static_cast<heep::HeapBlock*>(heep::helpers::get_block_advanced_by_size(vHeap, 3 *heep::helpers::page_size));
        auto [prev, next] = fList.findPrevAndNextFree(tempPtr);
        REQUIRE(prev == fList.getTail());
        REQUIRE(next == nullptr);
    }

    SECTION("When block has smallest address it should be prepended") {
        fList = heep::FreeList{
            heep::helpers::get_block_advanced_by_size((void*)vHeap, heep::helpers::page_size),
            heep::helpers::page_size
        };

        auto* tempPtr = static_cast<heep::HeapBlock*>(heep::helpers::get_block_advanced_by_size(vHeap,0));
        auto [prev, next] = fList.findPrevAndNextFree(tempPtr);
        REQUIRE(prev == nullptr);
        REQUIRE(next == fList.getHead());
    }

    SECTION("When block address within range it should have valid prev and next") {
        auto* head = fList.getHead();
        auto* m1 = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* middle = fList.pushBack(m1->getEndAddr(), heep::helpers::page_size);
        auto* m2 = fList.pushBack(middle->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(m2->getEndAddr(),heep::helpers::page_size);

        fList.erase(m1); m1->setFree(false);
        fList.erase(m2); m2->setFree(false);
        fList.erase(middle); middle->setFree(false);

        auto [prev, next] = fList.findPrevAndNextFree(middle);
        REQUIRE(prev == head);
        REQUIRE(next == tail);
    }
}

TEST_CASE("Free List should insert blocks and coalesce when necessary", "[freelist]")  {
    char vHeap[heep::helpers::page_size*10];
    void* vHeapEnd = heep::helpers::get_block_advanced_by_size(vHeap,heep::helpers::page_size*10);
    heep::FreeList fList{vHeap, heep::helpers::page_size};

    SECTION("Inserting into an empty list should push back") {
        fList = heep::FreeList{};
        auto* block = fList.insert((void*)vHeap, heep::helpers::page_size, (void*)vHeap, vHeapEnd);

        REQUIRE(fList.size() == heep::helpers::page_size);
        REQUIRE(fList.getHead() == block);
        REQUIRE(fList.getTail() == block);
    }

    SECTION("Inserting when coalesce right only should maintain start but adjust size") {
        auto* head = fList.getHead();
        auto* m1 = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* middle = fList.pushBack(m1->getEndAddr(), heep::helpers::page_size);
        auto* m2 = fList.pushBack(middle->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(m2->getEndAddr(),heep::helpers::page_size);


        size_t expectedSizeBlock = middle->getSize() + m2->getSize();
        size_t expectedSize = 4*heep::helpers::page_size;

        fList.erase(m1); m1->setFree(false);
        fList.erase(middle); middle->setFree(false);

        auto* block = fList.insert(middle, heep::helpers::page_size, vHeap, vHeapEnd);
        REQUIRE(block == middle);
        REQUIRE(block->getNext() == tail);
        REQUIRE(tail->getPrev() == block);
        REQUIRE(block->getPrev() == head);
        REQUIRE(head->getNext() == block);
        REQUIRE(block->getSize() == expectedSizeBlock);
        REQUIRE(fList.size() == expectedSize);
    }

    SECTION("Inserting when coalesce left only should change start and adjust size") {
        auto* head = fList.getHead();
        auto* m1 = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* middle = fList.pushBack(m1->getEndAddr(), heep::helpers::page_size);
        auto* m2 = fList.pushBack(middle->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(m2->getEndAddr(),heep::helpers::page_size);


        size_t expectedSizeBlock = middle->getSize() + m1->getSize();
        size_t expectedSize = 4*heep::helpers::page_size;

        fList.erase(m2); m2->setFree(false);
        fList.erase(middle); middle->setFree(false);

        auto* block = fList.insert(middle, heep::helpers::page_size, vHeap, vHeapEnd);
        REQUIRE(block == m1);
        REQUIRE(block->getNext() == tail);
        REQUIRE(tail->getPrev() == block);
        REQUIRE(block->getPrev() == head);
        REQUIRE(head->getNext() == block);
        REQUIRE(block->getSize() == expectedSizeBlock);
        REQUIRE(fList.size() == expectedSize);
    }

    SECTION("Inserting when coalesce both should change start and adjust size all") {
        auto* head = fList.getHead();
        auto* m1 = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* middle = fList.pushBack(m1->getEndAddr(), heep::helpers::page_size);
        auto* m2 = fList.pushBack(middle->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(m2->getEndAddr(),heep::helpers::page_size);

        size_t expectedSizeBlock = middle->getSize() + m1->getSize() + m2->getSize();
        size_t expectedSize = 5*heep::helpers::page_size;

        fList.erase(middle); middle->setFree(false);

        auto* block = fList.insert(middle, heep::helpers::page_size, vHeap, vHeapEnd);
        REQUIRE(block == m1);
        REQUIRE(block->getNext() == tail);
        REQUIRE(tail->getPrev() == block);
        REQUIRE(block->getPrev() == head);
        REQUIRE(head->getNext() == block);
        REQUIRE(block->getSize() == expectedSizeBlock);
        REQUIRE(fList.size() == expectedSize);
    }

    SECTION("Inserting when coalesce none should maintain start and size") {
        auto* head = fList.getHead();
        auto* m1 = fList.pushBack(head->getEndAddr(), heep::helpers::page_size);
        auto* middle = fList.pushBack(m1->getEndAddr(), heep::helpers::page_size);
        auto* m2 = fList.pushBack(middle->getEndAddr(), heep::helpers::page_size);
        auto* tail = fList.pushBack(m2->getEndAddr(),heep::helpers::page_size);


        size_t expectedSizeBlock = middle->getSize();
        size_t expectedSize = 3*heep::helpers::page_size;

        fList.erase(m1); m1->setFree(false);
        fList.erase(m2); m2->setFree(false);
        fList.erase(middle); middle->setFree(false);

        auto* block = fList.insert(middle, heep::helpers::page_size, vHeap, vHeapEnd);
        REQUIRE(block == middle);
        REQUIRE(block->getNext() == tail);
        REQUIRE(tail->getPrev() == block);
        REQUIRE(block->getPrev() == head);
        REQUIRE(head->getNext() == block);
        REQUIRE(block->getSize() == expectedSizeBlock);
        REQUIRE(fList.size() == expectedSize);
    }
}