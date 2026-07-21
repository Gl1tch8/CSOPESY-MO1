#pragma once

#include <cstddef>
#include <mutex>
#include <set>
#include <utility>

// Bookkeeping-only swap-space simulation: records which (pid, page) pairs
// are currently evicted from physical memory. This emulator has no
// per-byte memory-content model, so there is nothing real to write to a
// "swap file" — a page's presence in `records` IS its swapped-out state.
class BackingStore {
public:
    void store(int pid, size_t page);
    bool hasRecord(int pid, size_t page) const;
    void erase(int pid, size_t page);
    void purgeProcess(int pid);
    size_t recordCount() const;

private:
    std::set<std::pair<int, size_t>> records;
    mutable std::mutex mutex_;
};
