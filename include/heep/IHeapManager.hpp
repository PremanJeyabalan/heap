//
// Created by prema on 13/7/2024.
//

#ifndef HEEP_IHEAPMANAGER_HPP
#define HEEP_IHEAPMANAGER_HPP

#include <concepts>
#include "IFreeList.hpp"
#include "FreeList.hpp"

#include "IVirtualStaticHeap.hpp"
#include "VirtualStaticHeap.hpp"

#include "IFinders.hpp"

namespace heep {
    template<typename Derived, typename Finder>
    concept CanAllocate = requires(Derived d, size_t s) {
        { d.template allocate<Finder>(s) } -> std::same_as<void*>;
    };

    template <typename Derived>
    concept CanDeallocate = requires(Derived d, void* p) {
        { d.deallocate(p) } -> std::same_as<void>;
    };

    template <typename Derived>
    concept HasConstCapacity = requires(const Derived& d) {
        { d.capacity() } -> std::same_as<void>;
    };


    template <typename Derived>
    concept HeapManagerConcept =
            HasConstCapacity<Derived> && HasConstMethods<Derived> && CanDeallocate<Derived>;

    template <typename Derived, FreeListConcept FList, vmem::VirtualStaticHeapConcept V>
    class IHeapManager {
    public:
        IHeapManager() { static_assert(HeapManagerConcept<Derived> && std::derived_from<Derived, IHeapManager>, "Derived type does not satisfy IHeapManager concept"); }
        Derived* impl() { return static_cast<Derived*>(this); }

    public:
        template<finders::FindersConcept<FList> Finder>
        void* do_allocate(size_t s) {
            static_assert(CanAllocate<Derived, Finder>, "Derived type should use a valid Finder");
            return impl()->template allocate<Finder>(s);
        }

    protected:
        V m_heap;
        FList m_freeList;
    };

}

#endif //HEEP_IHEAPMANAGER_HPP
