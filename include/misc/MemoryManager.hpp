#pragma once

#include "FrameManager.hpp"
#include "PagingAllocator.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Orchestrator: owns the single shared FrameManager plus one PagingAllocator
// per resident process, and exposes pid-keyed convenience methods matching
// the scheduler's original MemoryAllocator call sites.
class MemoryManager {
public:
    MemoryManager();

    void configure(uint64_t maxOverallMem, uint32_t memPerFrame);

    bool isResident(int pid) const;
    bool allocate(int pid, const std::string& name, uint64_t size);
    void deallocate(int pid);

    struct FrameSnapshotView {
        size_t frameIndex;
        bool occupied;
        int pid;
        std::string processName; // "" if unoccupied
        size_t ownerPage;
    };
    std::vector<FrameSnapshotView> snapshotFrames() const; // ascending frame index
    int residentProcessCount() const;
    uint64_t totalFragmentation() const;         // free-frame bytes system-wide (drop-in for the old report field)
    uint64_t totalInternalFragmentation() const; // wasted tail-page bytes summed over residents
    uint64_t getTotalSize() const;
    uint64_t getNumPagedIn() const;
    uint64_t getNumPagedOut() const;

private:
    uint64_t maxOverallMem = 0;
    uint32_t memPerFrame = 16;
    FrameManager frameManager;
    std::unordered_map<int, std::unique_ptr<PagingAllocator>> processAllocators;
    std::unordered_map<int, std::string> processNames;
    mutable std::mutex mapMutex;
};
