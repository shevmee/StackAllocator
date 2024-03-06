#include "../include/StackAllocator.h"

#include <iostream>
#include <chrono>
#include <list>

constexpr std::size_t StackSize = 7168;
constexpr std::size_t N = 1000;

int main() {
    // Standard Allocator
    std::list<int> stdList;

    auto start = std::chrono::high_resolution_clock::now();
    
    // Sequential Insertion
    for (int i = 0; i < N; ++i) {
        stdList.push_back(i);
    }

    // Sequential Deletion
    for (int i = 0; i < N; ++i) {
        stdList.pop_back();
    }

    // Random Access Write
    for (int i = 0; i < N; ++i) {
        auto it = stdList.begin();
        std::advance(it, N / 2); // Modify element in the middle of the list
        *it = i;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_std = end - start;
    std::cout << "Standard Allocator Time: " << duration_std.count() << " seconds\n";


    // Short Allocator
    StackMemory<StackSize> myArena; // Adjust the arena size as needed
    StackAllocator<int, StackSize> shortAllocator(myArena);
    std::list<int, StackAllocator<int, StackSize>> shortList(shortAllocator);

    start = std::chrono::high_resolution_clock::now();
    
    // Sequential Insertion
    for (int i = 0; i < N; ++i) {
        shortList.push_back(i);
    }

    // Sequential Deletion
    for (int i = 0; i < N; ++i) {
        shortList.pop_back();
    }

    // Random Access Write
    for (int i = 0; i < N; ++i) {
        auto it = shortList.begin();
        std::advance(it, N / 2); // Modify element in the middle of the list
        *it = i;
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_short = end - start;
    std::cout << "Short Allocator Time: " << duration_short.count() << " seconds\n";
}