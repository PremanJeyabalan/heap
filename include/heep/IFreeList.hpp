//
// Created by prema on 13/7/2024.
//

#ifndef HEEP_IFREELIST_HPP
#define HEEP_IFREELIST_HPP

#include "HeapBlock.hpp"

namespace heep {
    template <typename Derived>
    concept CanPush = requires(Derived d, void* m, size_t s) {
        {d.pushBack(m, s)} -> std::same_as<HeapBlock*>;
        {d.pushFront(m, s)} -> std::same_as<HeapBlock*>;
    };

    template <typename Derived>
    concept CanInsert = requires(Derived d, void* m, size_t s, const void* start, const void* end) {
        {d.insert(m, s, start, end)} -> std::same_as<HeapBlock*>;
    };

    template <typename Derived>
    concept CanPop = requires(Derived d) {
        {d.popBack()} -> std::same_as<void>;
        {d.popFront()} -> std::same_as<void>;
    };

    template <typename Derived>
    concept CanResize = requires(Derived d, void* m, size_t s) {
        {d.resizeBlock(m, s)} -> std::same_as<std::optional<HeapBlock*>>;
    };

    template <typename Derived>
    concept CanErase = requires(Derived d, HeapBlock* b) {
        {d.erase(b)} -> std::same_as<void>;
    };

    template <typename Derived>
    concept HasConstMethods = requires(const Derived& d) {
        { d.print() } -> std::same_as<void>;
        { d.size() } -> std::same_as<size_t>;
    };

    template <typename Derived>
    concept HasConstGetHeadTail = requires(const Derived& d) {
        { d.getHead() } -> std::same_as<HeapBlock*>;
        { d.getTail() } -> std::same_as<HeapBlock*>;
    };

    template <typename Derived>
    concept FreeListConcept =
            CanPush<Derived> && CanInsert<Derived> && CanPop<Derived> && CanErase<Derived> && HasConstMethods<Derived> && HasConstGetHeadTail<Derived>;


    template <typename Derived>
    class IFreeList {
    public:
        IFreeList() { static_assert(FreeListConcept<Derived> && std::derived_from<Derived, IFreeList>);}
        Derived* impl() { return static_cast<Derived*>(this); }

    public:
        HeapBlock* do_pushBack(void* m, size_t s) { return impl()->pushBack(m, s); };
        HeapBlock* do_pushFront(void* m, size_t s) { return impl()->pushFront(m, s); };
        HeapBlock* do_insert(void* m, size_t s, const void* start, const void* end) { return impl()->insert(m, s, start, end); };
        std::optional<HeapBlock*> do_resizeBlock(HeapBlock* b, size_t s) { return impl()->resizeBlock(b, s); };
        void do_popFront() { return impl()->popFront(); }
        void do_popBack() { return impl()->popBack(); }
        void do_erase(HeapBlock* b) { return impl()->erase(b); }

    public:
        void do_print() { impl()->print(); };

    public:
        void do_size() { impl()->size(); };
        void do_getHead() { impl()->getHead(); };
        void do_getTail() { impl()->getTail(); };

    };

}

#endif //HEEP_IFREELIST_HPP
