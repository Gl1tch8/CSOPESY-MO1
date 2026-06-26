#pragma once

#include <vector>
#include <shared_mutex>
#include <algorithm>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>

#include "Process.hpp"
#include "Core.hpp"

class SystemState {
public:
    static SystemState& getInstance();

    // init before any other cmd is recognize
    bool isInitialized() const { return initialized; }
    void setInitialized(bool state) { initialized = state; }

    uint64_t getSystemTime() const { return globalTick.load(); }
    void incrementSystemTime() { globalTick++; }

    void initializeCores(int count);
    
    int getNumCores() const;
    const int getActiveCores() const;
    int getCoresUsed() const;
    int getCoresAvailable() const;
    double getCpuUtilization() const;

    void start();   // launch mainLoop on a background thread
    void stop();    // signal the loop to stop, then join

    void setCoreActive(int coreId, bool active);
    void setCoreProcess(int coreId, Process* p);

    void addProcess(const Process& process);
    const std::vector<std::shared_ptr<Process>>& getRunningProcesses() const;
    const std::vector<std::shared_ptr<Process>>& getFinishedProcesses() const;

    std::shared_ptr<Process> getProcessByPid(int pid);
    std::shared_ptr<Process> getProcessByName(const std::string& name);


private:
    SystemState();
    ~SystemState();

    void mainLoop();

    //prevent copying
    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    bool initialized = false;
    std::atomic<bool> isRunning = true;
    std::atomic<uint64_t> globalTick{0};

    std::thread loopThread;

    std::vector<Core> cores;
    // std::vector no work bc Process has a mutex (unmovable)
    std::vector<std::shared_ptr<Process>> processes;
    mutable std::shared_mutex processMutex;

};
