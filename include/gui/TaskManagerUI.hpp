#pragma once

#include <string>
#include <vector>

#include "imgui.h"

#include "AWindow.hpp"

// Component 3: a Windows-style Task Manager.
// Tabbed window with live Performance graphs and a sortable process table.
// All data is dummy/placeholder.
class TaskManagerUI : public AWindow {
public:
    TaskManagerUI();

    void draw() override;

private:
    // Dummy process row.
    enum class ProcessState { RUNNING, READY, WAITING, TERMINATED };
    struct Process {
        int pid;
        std::string name;
        ProcessState state;
        float cpuUsage;
        size_t memoryUsage; // KB
    };

    void drawPerformanceTab();
    void drawProcessesTab();
    void updatePerformanceData();
    void sortProcesses(ImGuiTableSortSpecs* specs);

    static ImVec4 getStateColor(ProcessState state);
    static const char* getStateName(ProcessState state);

    std::vector<float> cpuHistory;
    std::vector<float> memoryHistory;
    int tick = 0; // drives the deterministic dummy waveform

    std::vector<Process> processes;
};
