#include "../../include/misc/PagingAllocator.hpp"
#include "../../include/misc/FrameManager.hpp"

#include <algorithm>
#include <sstream>

PagingAllocator::PagingAllocator(int pid, size_t arenaSize, size_t frameSize, FrameManager* frameManager)
    : pid(pid), frameSize(frameSize), frameManager(frameManager) {
    memoryAllocatorType = PAGING;
    maximumSize = arenaSize;
    currentAllocatedSize = 0;
    size_t numPages = (arenaSize + frameSize - 1) / frameSize;
    pageTable.assign(numPages, PageTableEntry{});
    freeRanges.insert(MemoryBlock{0, arenaSize});
}

void* PagingAllocator::allocate(size_t size) {
    if (size == 0) return nullptr;

    size_t pagesNeeded = (size + frameSize - 1) / frameSize;
    if (pagesNeeded > frameManager->getTotalFrames()) return nullptr; // pathological: can never fit

    size_t offset, firstPage, lastPage;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        auto it = std::find_if(freeRanges.begin(), freeRanges.end(),
                                [size](const MemoryBlock& b) { return b.size >= size; });
        if (it == freeRanges.end()) return nullptr; // arena's own virtual space is full
        MemoryBlock chosen = *it;
        freeRanges.erase(it);
        offset = chosen.start;
        if (chosen.size > size) {
            freeRanges.insert(MemoryBlock{offset + size, chosen.size - size});
        }
        firstPage = offset / frameSize;
        lastPage = (offset + size - 1) / frameSize;
    }

    // Acquire frames OUTSIDE stateMutex: acquireFrame may evict another
    // process's page, which calls back into MemoryManager and possibly THIS
    // SAME PagingAllocator's invalidatePage() (a saturated system can select
    // one of this process's own still-valid pages as the FIFO victim while
    // resuming). Holding stateMutex across that call would self-deadlock.
    for (size_t p = firstPage; p <= lastPage; ++p) {
        size_t frame = frameManager->acquireFrame(pid, p);
        std::lock_guard<std::mutex> lock(stateMutex);
        pageTable[p] = PageTableEntry{true, frame};
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    liveAllocations[offset] = size;
    currentAllocatedSize += size;
    return reinterpret_cast<void*>(offset);
}

void PagingAllocator::deallocate(void* ptr) {
    size_t offset = reinterpret_cast<size_t>(ptr);
    size_t size, firstPage, lastPage;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        auto it = liveAllocations.find(offset);
        if (it == liveAllocations.end()) return; // unknown pointer / double-free guard
        size = it->second;
        liveAllocations.erase(it);
        currentAllocatedSize -= size;
        firstPage = offset / frameSize;
        lastPage = (offset + size - 1) / frameSize;
    }

    for (size_t p = firstPage; p <= lastPage; ++p) {
        size_t frameToRelease = SIZE_MAX;
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            if (pageTable[p].valid) {
                frameToRelease = pageTable[p].frameNumber;
                pageTable[p] = PageTableEntry{};
            }
        }
        if (frameToRelease != SIZE_MAX) {
            frameManager->releaseFrame(frameToRelease);
        }
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    insertAndCoalesce(MemoryBlock{offset, size});
}

int PagingAllocator::restoreEvictedPages() {
    std::vector<size_t> pagesToRestore;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        for (const auto& [start, size] : liveAllocations) {
            size_t firstPage = start / frameSize;
            size_t lastPage = (start + size - 1) / frameSize;
            for (size_t p = firstPage; p <= lastPage; ++p) {
                if (!pageTable[p].valid) pagesToRestore.push_back(p);
            }
        }
    }
    for (size_t p : pagesToRestore) {
        size_t frame = frameManager->acquireFrame(pid, p); // FrameManager decides paged-in vs. not
        std::lock_guard<std::mutex> lock(stateMutex);
        pageTable[p] = PageTableEntry{true, frame};
    }
    return static_cast<int>(pagesToRestore.size());
}

void PagingAllocator::invalidatePage(size_t page) {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (page < pageTable.size()) pageTable[page] = PageTableEntry{};
}

void PagingAllocator::releaseAllFrames() {
    std::vector<size_t> framesToRelease;
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        for (auto& entry : pageTable) {
            if (entry.valid) {
                framesToRelease.push_back(entry.frameNumber);
                entry = PageTableEntry{};
            }
        }
    }
    for (size_t f : framesToRelease) {
        frameManager->releaseFrame(f);
    }
}

bool PagingAllocator::isFullyResident() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (liveAllocations.empty()) return false; // never allocated => not resident
    for (const auto& [start, size] : liveAllocations) {
        size_t firstPage = start / frameSize;
        size_t lastPage = (start + size - 1) / frameSize;
        for (size_t p = firstPage; p <= lastPage; ++p) {
            if (!pageTable[p].valid) return false;
        }
    }
    return true;
}

size_t PagingAllocator::getPageCount() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return pageTable.size();
}

uint64_t PagingAllocator::getAllocatedBytes() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return currentAllocatedSize;
}

std::string PagingAllocator::visualizeMemory() {
    std::lock_guard<std::mutex> lock(stateMutex);
    std::ostringstream ss;
    ss << "PID " << pid << " arena=" << maximumSize << " allocated=" << currentAllocatedSize << "\n";
    for (size_t p = 0; p < pageTable.size(); ++p) {
        ss << "  page " << p << ": "
           << (pageTable[p].valid ? ("frame " + std::to_string(pageTable[p].frameNumber)) : "swapped-out")
           << "\n";
    }
    return ss.str();
}

void PagingAllocator::insertAndCoalesce(MemoryBlock block) { // caller holds stateMutex
    auto it = freeRanges.insert(block).first;
    if (it != freeRanges.begin()) {
        auto prev = std::prev(it);
        if (prev->start + prev->size == it->start) {
            MemoryBlock merged{prev->start, prev->size + it->size};
            freeRanges.erase(prev);
            freeRanges.erase(it);
            it = freeRanges.insert(merged).first;
        }
    }
    auto next = std::next(it);
    if (next != freeRanges.end() && it->start + it->size == next->start) {
        MemoryBlock merged{it->start, it->size + next->size};
        freeRanges.erase(it);
        freeRanges.erase(next);
        freeRanges.insert(merged);
    }
}
