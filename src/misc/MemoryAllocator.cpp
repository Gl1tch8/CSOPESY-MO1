#include "../../include/misc/MemoryAllocator.hpp"

void MemoryAllocator::configure(uint64_t totalSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    this->totalSize = totalSize;
    blocks.clear();
    blocks.push_back(MemoryBlock{0, totalSize, true, -1, ""});
}

bool MemoryAllocator::allocate(int pid, const std::string& name, uint64_t size) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (size_t i = 0; i < blocks.size(); ++i) {
        MemoryBlock& block = blocks[i];
        if (!block.free || block.size < size) continue;

        if (block.size == size) {
            block.free = false;
            block.pid = pid;
            block.processName = name;
        } else {
            MemoryBlock remainder{block.base + size, block.size - size, true, -1, ""};
            block.size = size;
            block.free = false;
            block.pid = pid;
            block.processName = name;
            blocks.insert(blocks.begin() + i + 1, remainder);
        }
        return true;
    }
    return false;
}

void MemoryAllocator::deallocate(int pid) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& block : blocks) {
        if (!block.free && block.pid == pid) {
            block.free = true;
            block.pid = -1;
            block.processName.clear();
            coalesceLocked();
            return;
        }
    }
}

void MemoryAllocator::coalesceLocked() {
    for (size_t i = 0; i + 1 < blocks.size();) {
        if (blocks[i].free && blocks[i + 1].free) {
            blocks[i].size += blocks[i + 1].size;
            blocks.erase(blocks.begin() + i + 1);
        } else {
            ++i;
        }
    }
}

bool MemoryAllocator::isResident(int pid) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& block : blocks) {
        if (!block.free && block.pid == pid) return true;
    }
    return false;
}

std::vector<MemoryAllocator::BlockView> MemoryAllocator::snapshotBlocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BlockView> result;
    result.reserve(blocks.size());
    for (const auto& block : blocks) {
        result.push_back(BlockView{block.base, block.size, block.free, block.pid, block.processName});
    }
    return result;
}

int MemoryAllocator::residentProcessCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    int count = 0;
    for (const auto& block : blocks) {
        if (!block.free) count++;
    }
    return count;
}

uint64_t MemoryAllocator::totalFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    uint64_t free = 0;
    for (const auto& block : blocks) {
        if (block.free) free += block.size;
    }
    return free;
}

uint64_t MemoryAllocator::getTotalSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalSize;
}
