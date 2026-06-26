#pragma once

#include <vector>
#include "Process.hpp"
#include "Core.hpp"

class SystemState {
public:
    static SystemState& getInstance();
    void initializeCores(int count);

    int getNumCores() const;
    const int getActiveCores() const;

    // CPU utilization as a percentage (used cores / total cores * 100)
    double getCpuUtilization() const;
    // number of cores currently running a process
    int getCoresUsed() const;
    // number of idle cores
    int getCoresAvailable() const;

    void addProcess(const Process& process);
    std::vector<Process> getRunningProcesses() const;
    std::vector<Process> getFinishedProcesses() const;


private:
    SystemState();
    ~SystemState() = default;

    //prevent copying
    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    std::vector<Core> cores;
    std::vector<Process> processes;
};
