#include "../../include/SystemState.hpp"
#include <algorithm>

// ==========================================
// SystemState Implementation
// ==========================================

SystemState& SystemState::getInstance() {
    static SystemState instance;
    return instance;
}

SystemState::SystemState() {
    loadMockData();
}

int SystemState::getNumCores() const {
    return static_cast<int>(cores.size());
}

void SystemState::initializeCores(int count) {
    cores.clear();
    for (int i = 0; i < count; ++i) {
        cores.emplace_back(i);
    }
}

int SystemState::getCoresUsed() const {
    int count = 0;
    for (const auto& core : cores) {
        if (core.isActive()) {
            count++;
        }
    }
    return count;
}

int SystemState::getCoresAvailable() const {
    int count = 0;
    for (const auto& core : cores) {
        if (!core.isActive()) {
            count++;
        }
    }
    return count;
}

float SystemState::getCpuUtilization() const {
    if (cores.empty()) return 0.0f;
    return (static_cast<float>(getCoresUsed()) / cores.size()) * 100.0f;
}

const std::vector<Process>& SystemState::getRunningProcesses() const {
    return runningProcesses;
}

const std::vector<Process>& SystemState::getFinishedProcesses() const {
    return finishedProcesses;
}

void SystemState::addRunningProcess(const Process& process) {
    runningProcesses.push_back(process);
}

void SystemState::addFinishedProcess(const Process& process) {
    finishedProcesses.push_back(process);
}

void SystemState::clearProcesses() {
    runningProcesses.clear();
    finishedProcesses.clear();
}

void SystemState::loadMockData() {
    clearProcesses();
    initializeCores(4);

    // Set all 4 cores to active for the mock data
    for (auto& core : cores) {
        core.setActive(true);
    }

    // Add running processes from PDF mockup
    addRunningProcess(Process("process05", 5, "01/18/2024 09:15:22AM", 0, 1235, 5876, false));
    addRunningProcess(Process("process06", 6, "01/18/2024 09:17:22AM", 1, 3, 5876, false));
    addRunningProcess(Process("process07", 7, "01/18/2024 09:17:45AM", 2, 9, 1000, false));
    addRunningProcess(Process("process08", 8, "01/18/2024 09:18:58AM", 3, 12, 80, false));

    // Add finished processes from PDF mockup
    addFinishedProcess(Process("process01", 1, "01/18/2024 09:00:21AM", -1, 5876, 5876, true));
    addFinishedProcess(Process("process02", 2, "01/18/2024 09:00:22AM", -1, 5876, 5876, true));
    addFinishedProcess(Process("process03", 3, "01/18/2024 09:00:42AM", -1, 1000, 1000, true));
    addFinishedProcess(Process("process04", 4, "01/18/2024 09:00:53AM", -1, 80, 80, true));
}
