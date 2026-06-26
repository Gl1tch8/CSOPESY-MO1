#include "../../include/misc/SystemState.hpp"
#include "Core.hpp"

#include <chrono>

SystemState::SystemState() {}

// ensure the background loop is stopped before destruction
SystemState::~SystemState() {
    stop();
}

// singleton instance of system state
SystemState& SystemState::getInstance() {
    static SystemState instance;
    return instance;
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

void SystemState::setCoreActive(int coreId, bool active) {
    if (coreId >= 0 && coreId < static_cast<int>(cores.size())) {
        cores[coreId].setActive(active);
    }
}

void SystemState::setCoreProcess(int coreId, Process* p) {
    if (coreId >= 0 && coreId < static_cast<int>(cores.size())) {
        cores[coreId].setActiveProcess(p);
    }
}

// read only
const std::vector<std::shared_ptr<Process>>& SystemState::getRunningProcesses() const {
    static std::vector<std::shared_ptr<Process>> list;
    list.clear();
    std::shared_lock lock(processMutex);
    for (const auto& p : this->processes) {
        if (p && p->getState() == ProcessState::RUNNING) {
            list.push_back(p);
        }
    }
    return list;
}
// read only
const std::vector<std::shared_ptr<Process>>& SystemState::getFinishedProcesses() const {
    static std::vector<std::shared_ptr<Process>> list;
    list.clear();
    std::shared_lock lock(processMutex);
    for (const auto& p : this->processes) {
        if (p && p->getState() == ProcessState::FINISHED) {
            list.push_back(p);
        }
    }
    return list;
}

const std::vector<std::shared_ptr<Process>>& SystemState::getAllProcesses() const {
    static std::vector<std::shared_ptr<Process>> list;
    list.clear();
    std::shared_lock lock(processMutex);
    for (const auto& p : this->processes) {
        if (p) {
            list.push_back(p);
        }
    }
    return list;
}



void SystemState::addProcess(const Process& process) {
    std::unique_lock lock(processMutex);
    this->processes.push_back(std::make_shared<Process>(process));
}
std::shared_ptr<Process> SystemState::getProcessByPid(int pid) {
    std::shared_lock lock(processMutex);
    for (const auto& p : processes) {
        if (p && p->getPid() == pid) return p;
    }
    return nullptr;
}
std::shared_ptr<Process> SystemState::getProcessByName(const std::string& name) {
    std::shared_lock lock(processMutex);
    for (auto& p : processes) {
        if (p && p->getName() == name) return p;
    }
    return nullptr;
}

void SystemState::start() {
    // guard against double-start
    if (loopThread.joinable()) return;

    isRunning = true;
    loopThread = std::thread(&SystemState::mainLoop, this);
}

void SystemState::stop() {
    isRunning = false;                 // signal the loop to exit
    if (loopThread.joinable())
        loopThread.join();             // wait for it to actually finish
}

void SystemState::mainLoop() {
    while (this->isRunning) {
        this->incrementSystemTime();
        // give each tick some duration instead of spinning a CPU core flat-out
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}