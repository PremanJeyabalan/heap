//
// Created by prema on 22/6/2024.
//

#ifndef HEEP_FINDERS_HPP
#define HEEP_FINDERS_HPP

#include "heep/HeapBlock.hpp"
#include "heep/FreeList.hpp"

namespace heep::finders {
    struct BestFit {
        std::optional<HeapBlock*> operator()(FreeList& fList, size_t size) {
            HeapBlock* curr = fList.getHead();
            if (curr == nullptr)
                return std::nullopt;

            HeapBlock* res{};
            while (curr != nullptr) {
                size_t currSize = curr->getSize();
                if (currSize == size)
                    return curr;
                else if (currSize > size && (res == nullptr || currSize < res->getSize()))
                    res = curr;

                curr = curr->getNext();
            }

            return res == nullptr ?
                std::nullopt : std::make_optional(res);
        }
    };

    struct FirstFit {
        std::optional<HeapBlock*> operator()(FreeList& fList, size_t size) {
            HeapBlock* curr = fList.getHead();

            while (curr != nullptr) {
                if (curr->getSize() >= size)
                    return curr;

                curr = curr->getNext();
            }

            return std::nullopt;
        }
    };
}

#endif //HEEP_FINDERS_HPP
