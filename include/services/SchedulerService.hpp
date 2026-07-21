#include "../../src/interfaces/Service.hpp"
#include "../../include/misc/Process.hpp"
#include "../../include/misc/MemoryManager.hpp"
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
    void initScheduler();   // bring CPU cores online (called at "initialize")
    void start();           // begin dummy-process generation ("scheduler-start")
    void stop();
    void run();

    // stop and join every background thread (cores + snapshot writer). Must
    // be called before program exit — otherwise these threads keep polling
    // the SystemState singleton while it's torn down during static
    // destruction, which is a use-after-destruction race.
    void shutdown();

    // create a single named process and enqueue it for execution.
    // returns "" on success, or an error message on failure.
    std::string createProcess(const std::string& name);

    // read-only access for VmstatService (needs frame occupancy + counters)
    const MemoryManager& getMemoryManager() const { return memoryManager; }

private:
    void generateProcessor();
    void runCpuCore(int coreId);

    // build a randomized instruction block for a process (count in [minIns, maxIns])
    std::vector<Instruction> generateInstructions(const std::string& name);

    // assign a core (per scheduling algo) and enqueue the process for execution
    void enqueueProcess(ProcessInfo info);

    // periodically (every quantum-cycles global ticks) dump memory state to
    // memory_stamp_<qq>.txt
    void snapshotWriter();
    void writeMemorySnapshot(uint64_t qq);

    Config config;

    std::atomic<bool> running = false;
    std::vector<std::queue<Process>> cpuReadyQueues;
    std::vector<std::thread> cpuThreads;
    std::thread generatorThread;
    mutable std::mutex queueMutex;

    MemoryManager memoryManager;
    std::thread snapshotThread;
    std::atomic<uint64_t> snapshotCounter = 0;
    
    // atomic cuz written from generator thread and read concurrently
    std::atomic<uint32_t> processCounter = 1000;
    std::atomic<int> nextCoreAssignment = 0;

    // shared RNG for instruction generation; guarded because both the generator
    // thread and the main thread (createProcess) can build instructions.
    std::mt19937 rng{std::random_device{}()};
    std::mutex genMutex;
};