//
// Created by prema on 13/7/2024.
//

#ifndef HEEP_IFINDERS_HPP
#define HEEP_IFINDERS_HPP

#include "IFreeList.hpp"

namespace heep::finders {
    template <typename Derived, typename FList>
    concept IsCallable = requires(Derived d, const FList& f, size_t s) {
        { d(f, s) } -> std::same_as<std::optional<HeapBlock*>>;
    };

    template<typename Derived, typename FList>
    concept FindersConcept =
            heep::FreeListConcept<FList> && IsCallable<Derived, FList>;

    template<typename Derived, FreeListConcept FList>
    struct IFinder {
    public:
        IFinder() { static_assert(FindersConcept<Derived, FList> && std::derived_from<Derived, IFinder>);}
        Derived* impl() { return static_cast<Derived*>(this); }

    public:
        std::optional<HeapBlock*> operator()(const FList& f, size_t s) {return impl()(f, s);}
    };
}

#endif //HEEP_IFINDERS_HPP
