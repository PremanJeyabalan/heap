//
// Created by prema on 13/7/2024.
//

#ifndef HEEP_IVIRTUALSTATICHEAP_HPP
#define HEEP_IVIRTUALSTATICHEAP_HPP
#include <concepts>
#include <cstdint>

namespace heep::vmem {

    template <typename Derived>
    concept NotMoveable = !std::move_constructible<Derived>;

    template <typename Derived>
    concept CanRequest = requires(Derived d, std::size_t s) {
        { d.request(s) } -> std::same_as<void*>;
    };

    template <typename Derived>
    concept VirtualStaticHeapConcept =
            NotMoveable<Derived> && CanRequest<Derived>;

    template <typename Derived>
    class IVirtualStaticHeap {
        IVirtualStaticHeap() { static_assert(VirtualStaticHeapConcept<Derived> && std::derived_from<Derived, IVirtualStaticHeap>);}
        Derived* impl() { return static_cast<Derived*>(this); }

    public:
        void* do_request(std::size_t s) { return impl()->request(s); }
    };
}

#endif //HEEP_IVIRTUALSTATICHEAP_HPP
