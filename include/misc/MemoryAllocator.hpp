#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

struct MemoryBlock {
    uint64_t base;
    uint64_t size;
    bool free;
    int pid;                 // -1 if free
    std::string processName; // empty if free
};

// Generic first-fit contiguous allocator over a single fixed-size arena
// [0, totalSize). Blocks are kept sorted by base and always partition the
// arena with no implicit gaps — a "free" block IS the gap.
class MemoryAllocator {
public:
    // Resets the arena to a single free block covering [0, totalSize).
    void configure(uint64_t totalSize);

    // First-fit: scans blocks base-ascending, picks the first free block
    // with size >= `size`, splitting off the remainder as a new free block.
    // Returns false (no mutation) if no block fits.
    bool allocate(int pid, const std::string& name, uint64_t size);

    // Frees the block owned by pid (no-op if not resident) and coalesces
    // with adjacent free neighbors.
    void deallocate(int pid);

    bool isResident(int pid) const;

    struct BlockView {
        uint64_t base;
        uint64_t size;
        bool free;
        int pid;
        std::string name;
    };
    std::vector<BlockView> snapshotBlocks() const; // ascending base order
    int residentProcessCount() const;
    uint64_t totalFragmentation() const; // totalSize - sum(allocated sizes)
    uint64_t getTotalSize() const;

private:
    uint64_t totalSize = 0;
    std::vector<MemoryBlock> blocks;
    mutable std::mutex mutex_;

    void coalesceLocked(); // merge adjacent free blocks
};
