#pragma once

#include "BackingStore.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <vector>

// The single shared physical-frame pool for the whole system: exactly one
// instance, owned by MemoryManager. Frames are fixed-size and
// interchangeable, so (unlike a per-process virtual arena) there is no
// variable-size-gap coalescing to do here — a free-index deque is enough.
class FrameManager {
public:
    using EvictCallback = std::function<void(int pid, size_t page)>;

    void configure(size_t totalFrames);
    void setEvictCallback(EvictCallback cb);

    // Acquire a frame for (pid, page): reuse a free frame if one exists,
    // otherwise evict the oldest occupied frame (FIFO victim). If
    // (pid, page) has a prior backing-store record (this exact page was
    // evicted earlier and is now being restored), the record is consumed
    // and numPagedIn increments; a first-touch acquisition does not.
    size_t acquireFrame(int pid, size_t page);

    // Give a frame back to the free pool with NO backing-store write —
    // used when a process is dying, not being paged out.
    void releaseFrame(size_t frameIndex);

    // Drop any backing-store record still held for a dying pid.
    void purgeProcess(int pid);

    size_t getTotalFrames() const;
    size_t getUsedFrameCount() const;
    uint64_t getNumPagedIn() const;
    uint64_t getNumPagedOut() const;

    struct FrameView {
        bool occupied;
        int ownerPid;
        size_t ownerPage;
    };
    std::vector<FrameView> snapshotFrames() const; // ascending frame index

private:
    struct FrameEntry {
        bool occupied = false;
        int ownerPid = -1;
        size_t ownerPage = 0;
    };

    size_t evictVictimLocked(); // caller holds mutex_

    std::vector<FrameEntry> frames;
    std::deque<size_t> freeList;
    std::deque<size_t> fifoOrder; // occupied frame indices, oldest-acquired first
    BackingStore backingStore;
    std::atomic<uint64_t> numPagedIn{0};
    std::atomic<uint64_t> numPagedOut{0};
    EvictCallback onEvict;
    mutable std::mutex mutex_;
};
