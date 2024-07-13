//
// Created by prema on 22/6/2024.
//

#ifndef HEEP_FINDERS_HPP
#define HEEP_FINDERS_HPP

#include "heep/HeapBlock.hpp"
#include "heep/FreeList.hpp"

#include "IFinders.hpp"
#include "IFreeList.hpp"

namespace heep::finders {

    template<FreeListConcept FList>
    struct BestFit : public IFinder<BestFit<FList>, FList> {
        std::optional<HeapBlock*> operator()(const FList& fList, size_t size) const {
            HeapBlock* curr = fList.getHead();
            if (curr == nullptr)
                return std::nullopt;

            const size_t minFreeSize = size + 64;
            HeapBlock* res{};
            while (curr != nullptr) {
                size_t currSize = curr->getSize();
                if (currSize == size)
                    return curr;
                else if (currSize >= minFreeSize && (res == nullptr || currSize < res->getSize()))
                    res = curr;

                curr = curr->getNext();
            }

            return res == nullptr ?
                std::nullopt : std::make_optional(res);
        }
    };

    template<FreeListConcept FList>
    struct FirstFit : public IFinder<FirstFit<FList>, FList> {
        std::optional<HeapBlock*> operator()(const FList& fList, size_t size) const {
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
