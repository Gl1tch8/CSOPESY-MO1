#pragma once

#include <vector>
#include "Process.hpp"
#include "Core.hpp"

class SystemState {
public:
    static SystemState& getInstance();

    int getNumCores() const;
    void initializeCores(int count);

    int getCoresUsed() const;
    int getCoresAvailable() const;

    float getCpuUtilization() const;

    const std::vector<Process>& getRunningProcesses() const;
    const std::vector<Process>& getFinishedProcesses() const;

    void addRunningProcess(const Process& process);
    void addFinishedProcess(const Process& process);
    void clearProcesses();

    // Helper to populate initial mockup data matching PDF page 5
    void loadMockData();

private:
    SystemState();
    ~SystemState() = default;

    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    std::vector<Core> cores;
    std::vector<Process> runningProcesses;
    std::vector<Process> finishedProcesses;
};
