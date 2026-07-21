#pragma once

#include "IMemoryAllocator.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

class FrameManager;

// One instance per resident process: owns that process's own virtual arena
// [0, arenaSize) plus its flat page table into shared physical frames,
// reached via a raw back-pointer (FrameManager, owned by the same
// MemoryManager, outlives every PagingAllocator).
class PagingAllocator : public IMemoryAllocator {
public:
    struct PageTableEntry {
        bool valid = false;
        size_t frameNumber = 0;
    };

    PagingAllocator(int pid, size_t arenaSize, size_t frameSize, FrameManager* frameManager);

    // ---- IMemoryAllocator ----
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    std::string visualizeMemory() override;

    // ---- MemoryManager-only orchestration (not part of IMemoryAllocator) ----
    bool isFullyResident() const;     // true iff every page ever allocated is currently valid
    int restoreEvictedPages();        // re-acquire frames for invalid-but-allocated pages; returns count restored
    void invalidatePage(size_t page); // called by FrameManager's eviction callback
    void releaseAllFrames();          // process death: give back every frame held, no backing-store write
    size_t getPageCount() const;
    uint64_t getAllocatedBytes() const; // == currentAllocatedSize, exposed publicly

private:
    void insertAndCoalesce(MemoryBlock block); // caller must hold stateMutex

    int pid;
    size_t frameSize;
    FrameManager* frameManager;

    std::vector<PageTableEntry> pageTable;
    std::set<MemoryBlock> freeRanges;         // free VIRTUAL ranges within this process's own arena
    std::map<size_t, size_t> liveAllocations; // virtual start -> size; decodes a deallocate() ptr

    mutable std::mutex stateMutex; // guards pageTable / freeRanges / liveAllocations / currentAllocatedSize
};
