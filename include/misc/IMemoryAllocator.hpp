#pragma once

#include <cstddef>
#include <string>

// Abstract paging-capable memory allocator interface. A concrete allocator
// owns one bounded arena and hands out/reclaims ranges within it via
// malloc-like allocate(size)/deallocate(ptr) calls.
class IMemoryAllocator {
public:
    enum MemoryAllocatorType { FLAT_MEMORY_ALLOCATOR, PAGING };

    virtual ~IMemoryAllocator() = default;

    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual std::string visualizeMemory() = 0;

protected:
    MemoryAllocatorType memoryAllocatorType;

    struct MemoryBlock {
        size_t start;
        size_t size;
        bool operator<(const MemoryBlock& other) const { return start < other.start; }
    };

    size_t maximumSize;
    size_t currentAllocatedSize;
};
