#include "../../include/misc/BackingStore.hpp"

void BackingStore::store(int pid, size_t page) {
    std::lock_guard<std::mutex> lock(mutex_);
    records.insert({pid, page});
}

bool BackingStore::hasRecord(int pid, size_t page) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records.count({pid, page}) > 0;
}

void BackingStore::erase(int pid, size_t page) {
    std::lock_guard<std::mutex> lock(mutex_);
    records.erase({pid, page});
}

void BackingStore::purgeProcess(int pid) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = records.begin(); it != records.end();) {
        if (it->first == pid) {
            it = records.erase(it);
        } else {
            ++it;
        }
    }
}

size_t BackingStore::recordCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records.size();
}
