#pragma once

#include <vector>
#include <shared_mutex>
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
    const std::vector<Process>& getRunningProcesses() const;
    const std::vector<Process>& getFinishedProcesses() const;
    Process* getProcessByPid(int pid);
    Process* getProcessByName(const std::string& name);


private:
    SystemState();
    ~SystemState() = default;

    //prevent copying
    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    std::vector<Core> cores;
    std::vector<Process> processes;
    mutable std::shared_mutex processMutex;

};
