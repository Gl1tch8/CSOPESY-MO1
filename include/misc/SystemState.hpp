#pragma once

#include <vector>
#include <shared_mutex>
#include <algorithm>
#include <vector>
#include <mutex>

#include "Process.hpp"
#include "Core.hpp"

class SystemState {
public:
    static SystemState& getInstance();
    void initializeCores(int count);

    int getNumCores() const;
    const int getActiveCores() const;
    int getCoresUsed() const;
    int getCoresAvailable() const;
    double getCpuUtilization() const;

    void addProcess(const Process& process);
    const std::vector<std::shared_ptr<Process>>& getRunningProcesses() const;
    const std::vector<std::shared_ptr<Process>>& getFinishedProcesses() const;

    std::shared_ptr<Process> getProcessByPid(int pid);
    std::shared_ptr<Process> getProcessByName(const std::string& name);


private:
    SystemState();
    ~SystemState() = default;

    //prevent copying
    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    std::vector<Core> cores;
    // std::vector<Process> processes;
    // std::vector no work bc Process has a mutex (unmovable)
    std::vector<std::shared_ptr<Process>> processes;
    mutable std::shared_mutex processMutex;

};
