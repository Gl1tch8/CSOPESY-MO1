#include "../../include/misc/SystemState.hpp"
#include "Core.hpp"
#include <algorithm>
#include <vector>
#include <mutex>

SystemState::SystemState() {}

// singleton instance of system state
SystemState& SystemState::getInstance() {
    static SystemState instance;
    return instance;
}

void SystemState::addProcess(const Process& process) {
    std::unique_lock lock(processMutex);
    this->processes.push_back(process);
}

// read only
const std::vector<Process>& SystemState::getRunningProcesses() const {
    static std::vector<Process> list;
    list.clear();
    std::shared_lock lock(processMutex);
    for (int i = 0; i < static_cast<int>(this->processes.size()); i++) {
        if (processes[i].getState() == ProcessState::RUNNING) {
            list.push_back(processes[i]);
        }
    }
    return list;
}
// read only
const std::vector<Process>& SystemState::getFinishedProcesses() const {
    static std::vector<Process> list;
    list.clear();
    std::shared_lock lock(processMutex);
    for (int i = 0; i < static_cast<int>(this->processes.size()); i++) {
        if (processes[i].getState() == ProcessState::FINISHED) {
            list.push_back(processes[i]);
        }
    }
    return list;
}

// counts available cores
const int SystemState::getActiveCores() const {
    int j = 0;
    for(int i=0; i<this->cores.size(); i++) {
        if(this->cores[i].isActive()) {
            j+=1;
        }
    }
    return j;
}


int SystemState::getCoresUsed() const {
    return getActiveCores();
}

int SystemState::getCoresAvailable() const {
    return static_cast<int>(cores.size()) - getActiveCores();
}

// calculates CPU utilization as a percentage of running processes over total cores
double SystemState::getCpuUtilization() const {
    if (cores.empty()) return 0.0;
    int running = static_cast<int>(getRunningProcesses().size());
    return (static_cast<double>(running) / static_cast<double>(cores.size())) * 100.0;
}

void SystemState::initializeCores(int count) {
    cores.clear();

    for (int i = 0; i < count; i++) {
        Core core(i);
        core.setActive(false);
        this->cores.push_back(core);
    }
}

int SystemState::getNumCores() const {
    return this->cores.size();
}

Process* SystemState::getProcessByPid(int pid) {
    std::shared_lock lock(processMutex);
    for (int i = 0; i < static_cast<int>(processes.size()); i++) {
        if (processes[i].getPid() == pid) {
            return &processes[i];
        }
    }
    return nullptr;
}

Process* SystemState::getProcessByName(const std::string& name) {
    std::shared_lock lock(processMutex);
    for (auto& p : processes) {
        if (p.getName() == name) return &p;
    }
    return nullptr;
}