#include "../../include/services/VmstatService.hpp"
#include "../../include/services/SchedulerService.hpp"
#include "../../include/misc/MemoryManager.hpp"

#include <sstream>

VmstatService::VmstatService() {}

std::string VmstatService::executeFlags(std::string input) {
    if (!scheduler) {
        return "Error: scheduler not initialized.";
    }

    const MemoryManager& mm = scheduler->getMemoryManager();

    std::ostringstream ss;
    ss << "Total memory: " << mm.getTotalSize() << " bytes\n";
    ss << "Used memory: " << (mm.getTotalSize() - mm.totalFragmentation()) << " bytes\n";
    ss << "Free memory: " << mm.totalFragmentation() << " bytes\n";
    ss << "Internal fragmentation: " << mm.totalInternalFragmentation() << " bytes\n";
    ss << "Resident processes: " << mm.residentProcessCount() << "\n";
    ss << "Pages paged in: " << mm.getNumPagedIn() << "\n";
    ss << "Pages paged out: " << mm.getNumPagedOut() << "\n";
    ss << "-----------------------------------------\n";
    for (const auto& frame : mm.snapshotFrames()) {
        ss << "frame " << frame.frameIndex << ": ";
        if (frame.occupied) {
            ss << frame.processName << " (pid " << frame.pid << ", page " << frame.ownerPage << ")";
        } else {
            ss << "free";
        }
        ss << "\n";
    }
    return ss.str();
}
