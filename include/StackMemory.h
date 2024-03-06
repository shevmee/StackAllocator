#ifndef STACK_MEMORY_H
#define STACK_MEMORY_H

#include <cassert>
#include <cstddef>
#include <memory>   

template <std::size_t N, std::size_t alignment = alignof(std::max_align_t)>
class StackMemory {
    alignas(alignment) char _memory[N];
    char* _ptr;

public:
    StackMemory() noexcept : _ptr(_memory) {}
    ~StackMemory() {
        _ptr = nullptr;
    }

    StackMemory(const StackMemory&) = delete;
    StackMemory& operator=(const StackMemory&) = delete;

    template <std::size_t ReqAlign> 
    char* allocate_on_stack(std::size_t size_in_bits);

    void deallocate_on_stack(char* ptr, std::size_t size_in_bits) noexcept;

    static constexpr std::size_t memory_size() noexcept {
        return N;
    }

    std::size_t used() const noexcept {
        std::ptrdiff_t distance = _ptr - _memory;
        return static_cast<size_t>(distance);
    }

    void reset() noexcept {
        _ptr = _memory;
    }

private:
    static std::size_t align_up(std::size_t size_in_bits) noexcept {
        return (size_in_bits + (alignment-1)) & ~(alignment-1);
    }

    bool pointer_in_buffer(char* ptr) noexcept {
        return _memory <= ptr && ptr <= _memory + N;
    }
};

template <std::size_t N, std::size_t alignment>
template <std::size_t ReqAlign> 
char* StackMemory<N, alignment>::allocate_on_stack(std::size_t size_in_bits) {
    static_assert(ReqAlign <= alignment, "alignment is too small for this arena");
    assert(pointer_in_buffer(_ptr) && "StackAllocator has outlived arena");

    auto const aligned_size = align_up(size_in_bits);
    auto current_pos = static_cast<decltype(aligned_size)>(_memory + N - _ptr);
    if (current_pos >= aligned_size) {
        char* new_pos = _ptr;
        _ptr += aligned_size;
        return new_pos;
    }

    static_assert(alignment <= alignof(std::max_align_t), "you've chosen an "
                  "alignment that is larger than alignof(std::max_align_t), and "
                  "cannot be guaranteed by normal operator new");
    return static_cast<char*>(::operator new(size_in_bits));
}

template <std::size_t N, std::size_t alignment>
void StackMemory<N, alignment>::deallocate_on_stack(char* ptr, std::size_t size_in_bits) noexcept {
    assert(pointer_in_buffer(_ptr) && "StackAllocator has outlived arena");
    if (pointer_in_buffer(ptr)) {
        size_in_bits = align_up(size_in_bits);
        if (ptr + size_in_bits == _ptr)
        _ptr = ptr;
    }
    else
        ::operator delete(ptr);
}

#endif  // STACK_MEMORY_H