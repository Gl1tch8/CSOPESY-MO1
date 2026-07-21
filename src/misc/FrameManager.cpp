#include "../../include/misc/FrameManager.hpp"

#include <algorithm>

void FrameManager::configure(size_t totalFrames) {
    std::lock_guard<std::mutex> lock(mutex_);
    frames.assign(totalFrames, FrameEntry{});
    freeList.clear();
    for (size_t i = 0; i < totalFrames; ++i) {
        freeList.push_back(i);
    }
    fifoOrder.clear();
    numPagedIn = 0;
    numPagedOut = 0;
}

void FrameManager::setEvictCallback(EvictCallback cb) {
    onEvict = std::move(cb);
}

size_t FrameManager::evictVictimLocked() {
    size_t victim = fifoOrder.front();
    fifoOrder.pop_front();
    FrameEntry evicted = frames[victim];
    backingStore.store(evicted.ownerPid, evicted.ownerPage);
    numPagedOut++;
    frames[victim] = FrameEntry{};
    // Safe to call while holding mutex_: onEvict only touches
    // MemoryManager's map mutex and the victim PagingAllocator's own state
    // mutex, neither of which ever calls back into FrameManager.
    if (onEvict) {
        onEvict(evicted.ownerPid, evicted.ownerPage);
    }
    return victim;
}

size_t FrameManager::acquireFrame(int pid, size_t page) {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t frame;
    if (!freeList.empty()) {
        frame = freeList.front();
        freeList.pop_front();
    } else {
        frame = evictVictimLocked();
    }

    if (backingStore.hasRecord(pid, page)) {
        backingStore.erase(pid, page);
        numPagedIn++;
    }

    frames[frame] = FrameEntry{true, pid, page};
    fifoOrder.push_back(frame);
    return frame;
}

void FrameManager::releaseFrame(size_t frameIndex) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(fifoOrder.begin(), fifoOrder.end(), frameIndex);
    if (it != fifoOrder.end()) {
        fifoOrder.erase(it);
    }
    frames[frameIndex] = FrameEntry{};
    freeList.push_back(frameIndex);
}

void FrameManager::purgeProcess(int pid) {
    backingStore.purgeProcess(pid); // BackingStore self-protects; no reentry into FrameManager
}

size_t FrameManager::getTotalFrames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frames.size();
}

size_t FrameManager::getUsedFrameCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return frames.size() - freeList.size();
}

uint64_t FrameManager::getNumPagedIn() const { return numPagedIn.load(); }
uint64_t FrameManager::getNumPagedOut() const { return numPagedOut.load(); }

std::vector<FrameManager::FrameView> FrameManager::snapshotFrames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<FrameView> result;
    result.reserve(frames.size());
    for (const auto& f : frames) {
        result.push_back(FrameView{f.occupied, f.ownerPid, f.ownerPage});
    }
    return result;
}
