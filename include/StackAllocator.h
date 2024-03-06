#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

#include "StackMemory.h"

template <typename T, std::size_t N, std::size_t Align = alignof(std::max_align_t)>
class StackAllocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;

    static auto constexpr alignment = Align;
    static auto constexpr size = N;

    using arena_type = StackMemory<size, alignment>;
    using size_type = decltype(size);
private:
    arena_type& _arena;
public:
    StackAllocator(const StackAllocator&) = default;
    StackAllocator& operator=(const StackAllocator&) = delete;

    StackAllocator(arena_type& a) noexcept : _arena(a)
    {
        static_assert(size % alignment == 0,
                      "size needs to be a multiple of alignment");
    }
    template <typename U>
        StackAllocator(const StackAllocator<U, size, alignment>& sa) noexcept
            : _arena(sa._arena) {}

    template <typename U> 
    struct rebind {
        using other = StackAllocator<U, N, alignment>;
    };

    T* allocate(std::size_t size_in_bits) {
        return reinterpret_cast<T*>(_arena.template allocate_on_stack<alignof(T)>(size_in_bits * sizeof(T)));
    }
    
    void deallocate(T* p, std::size_t size_in_bits) noexcept {
        _arena.deallocate_on_stack(reinterpret_cast<char*>(p), size_in_bits * sizeof(T));
    }

    template <typename T1, std::size_t N1, std::size_t A1, typename T2, std::size_t N2, std::size_t A2>
    friend bool operator==(const StackAllocator<T1, N1, A1>& x, const StackAllocator<T2, N2, A2>& y) noexcept;

    template <typename T2, std::size_t N2, std::size_t A> 
    friend class StackAllocator;
};

template <typename T1, std::size_t N1, std::size_t A1, typename T2, std::size_t N2, std::size_t A2>
bool operator==(const StackAllocator<T1, N1, A1>& x, const StackAllocator<T2, N2, A2>& y) noexcept {
    return N1 == N2 && A1 == A2 && &x._arena == &y._arena;
}

template <typename T1, std::size_t N1, std::size_t A1, typename T2, std::size_t N2, std::size_t A2>
bool operator!=(const StackAllocator<T1, N1, A1>& x, const StackAllocator<T2, N2, A2>& y) noexcept {
    return !(x == y);
}

#endif