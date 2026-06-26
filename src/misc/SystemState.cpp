#include "../../include/misc/SystemState.hpp"
#include "Core.hpp"
#include <algorithm>
#include <vector>
SystemState::SystemState() {}

// singleton instance of system state
SystemState& SystemState::getInstance() {
    static SystemState instance;
    return instance;
}

void SystemState::addProcess(const Process& process) {
    this->processes.push_back(process);
}

std::vector<Process> SystemState::getRunningProcesses() const {
    std::vector<Process> list;
    for(int i=0; i<this->processes.size(); i++) {
        if(processes[i].getState() == ProcessState::RUNNING )
            list.push_back(processes[i]);
    }
    return list;
}

std::vector<Process> SystemState::getFinishedProcesses() const {
    std::vector<Process> list;
    for(int i=0; i<this->processes.size(); i++) {
        if(processes[i].getState() == ProcessState::FINISHED)
            list.push_back(processes[i]);
    }
    return list;
}

// number of cores currently running a process
int SystemState::getCoresUsed() const {
    return this->getActiveCores();
}

// number of idle cores
int SystemState::getCoresAvailable() const {
    return this->getNumCores() - this->getActiveCores();
}

// CPU utilization as a percentage (used cores / total cores * 100)
double SystemState::getCpuUtilization() const {
    int total = this->getNumCores();
    if (total == 0) return 0.0;
    return (static_cast<double>(this->getActiveCores()) / total) * 100.0;
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

void SystemState::initializeCores(int count) {
    
    for(int i=0; i<count; i++) {
        Core core;
        core.setActive(false);
        this->cores.push_back(core);
    }
}

int SystemState::getNumCores() const {
    return this->cores.size();
}