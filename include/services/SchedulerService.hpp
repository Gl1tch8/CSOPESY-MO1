#include "../../src/interfaces/Service.hpp"
#include "../../include/misc/Process.hpp"
#include "ConfigService.hpp"
#include <string>
#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <random>

class SchedulerService : public Service {
public:
    std::atomic<bool> generating = false;
    SchedulerService();
    std::string executeFlags(std::string input);
    void start();
    void stop();
    void run();

private:
    void generateProcessor();
    void runCpuCore(int coreId);

    Config config;

    std::atomic<bool> running = false;
    std::vector<std::queue<Process>> cpuReadyQueues;
    std::vector<std::thread> cpuThreads;
    std::thread generatorThread;
    mutable std::mutex queueMutex;
    
    // atomic cuz written from generator thread and read concurrently
    std::atomic<uint32_t> processCounter = 1000;
    std::atomic<int> nextCoreAssignment = 0;
};