#include "../../include/misc/MemoryManager.hpp"

MemoryManager::MemoryManager() {
    frameManager.setEvictCallback([this](int victimPid, size_t page) {
        std::lock_guard<std::mutex> lock(mapMutex); // safe: caller (below) never holds mapMutex across this
        auto it = processAllocators.find(victimPid);
        if (it != processAllocators.end()) it->second->invalidatePage(page);
    });
}

void MemoryManager::configure(uint64_t maxOverallMemIn, uint32_t memPerFrameIn) {
    maxOverallMem = maxOverallMemIn;
    memPerFrame = memPerFrameIn;
    size_t numFrames = static_cast<size_t>(maxOverallMem / memPerFrame);
    frameManager.configure(numFrames);
    std::lock_guard<std::mutex> lock(mapMutex);
    processAllocators.clear();
    processNames.clear();
}

bool MemoryManager::isResident(int pid) const {
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = processAllocators.find(pid);
    if (it == processAllocators.end()) return false;
    return it->second->isFullyResident();
}

bool MemoryManager::allocate(int pid, const std::string& name, uint64_t size) {
    PagingAllocator* target = nullptr;
    bool isNew = false;
    {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto it = processAllocators.find(pid);
        if (it == processAllocators.end()) {
            auto alloc = std::make_unique<PagingAllocator>(
                pid, static_cast<size_t>(size), static_cast<size_t>(memPerFrame), &frameManager);
            target = alloc.get();
            processAllocators.emplace(pid, std::move(alloc));
            processNames[pid] = name;
            isNew = true;
        } else {
            target = it->second.get();
        }
    } // release mapMutex BEFORE crossing into PagingAllocator/FrameManager

    if (isNew) {
        void* ptr = target->allocate(size);
        if (!ptr) {
            std::lock_guard<std::mutex> lock(mapMutex);
            processAllocators.erase(pid);
            processNames.erase(pid);
            return false;
        }
        return true;
    }

    target->restoreEvictedPages();
    return true;
}

void MemoryManager::deallocate(int pid) {
    std::unique_ptr<PagingAllocator> alloc;
    {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto it = processAllocators.find(pid);
        if (it == processAllocators.end()) return;
        alloc = std::move(it->second);
        processAllocators.erase(it);
        processNames.erase(pid);
    }
    alloc->releaseAllFrames();
    frameManager.purgeProcess(pid);
}

std::vector<MemoryManager::FrameSnapshotView> MemoryManager::snapshotFrames() const {
    auto raw = frameManager.snapshotFrames();
    std::lock_guard<std::mutex> lock(mapMutex);
    std::vector<FrameSnapshotView> result;
    result.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        std::string name;
        if (raw[i].occupied) {
            auto it = processNames.find(raw[i].ownerPid);
            if (it != processNames.end()) name = it->second;
        }
        result.push_back(FrameSnapshotView{i, raw[i].occupied, raw[i].ownerPid, name, raw[i].ownerPage});
    }
    return result;
}

int MemoryManager::residentProcessCount() const {
    std::lock_guard<std::mutex> lock(mapMutex);
    int count = 0;
    for (const auto& [pid, alloc] : processAllocators) {
        if (alloc->isFullyResident()) count++;
    }
    return count;
}

uint64_t MemoryManager::totalFragmentation() const {
    size_t freeFrames = frameManager.getTotalFrames() - frameManager.getUsedFrameCount();
    return static_cast<uint64_t>(freeFrames) * memPerFrame;
}

uint64_t MemoryManager::totalInternalFragmentation() const {
    std::lock_guard<std::mutex> lock(mapMutex);
    uint64_t total = 0;
    for (const auto& [pid, alloc] : processAllocators) {
        uint64_t committed = static_cast<uint64_t>(alloc->getPageCount()) * memPerFrame;
        total += committed - alloc->getAllocatedBytes();
    }
    return total;
}

uint64_t MemoryManager::getTotalSize() const { return maxOverallMem; }
uint64_t MemoryManager::getNumPagedIn() const { return frameManager.getNumPagedIn(); }
uint64_t MemoryManager::getNumPagedOut() const { return frameManager.getNumPagedOut(); }
