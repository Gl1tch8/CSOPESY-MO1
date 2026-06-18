#include "Scheduler.h"

Scheduler::Scheduler() {
}

Scheduler* Scheduler::singleton = nullptr;

void Scheduler::initialize() {
    singleton = new Scheduler();
}

Scheduler* Scheduler::getInstance() {
    return singleton;
}

void Scheduler::destroy() {
    delete singleton;
}

void Scheduler::start() {
    if (started) {
        return;
    }

    if (config.numCpu == 0) {
        config.numCpu = 4;
    }

    cpuReadyQueues.resize(config.numCpu);
    running = true;
    generatorRunning = true;

    for (uint8_t coreNumber = 0; coreNumber < config.numCpu; ++coreNumber) {
        cpuThreads.emplace_back(&Scheduler::runScheduler, this, coreNumber);
    }

    schedulerThread = std::thread(&Scheduler::generateMultipleProcesses, this);
    started = true;
}

void Scheduler::stop() {
    running = false;
    generatorRunning = false;

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    for (auto& t : cpuThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Scheduler::generateSingleProcess(std::string processName) {
    std::lock_guard<std::mutex> lock(counter_mutex);

    uint32_t processId = processCounter++;
    uint8_t assignedCore = coreCounter;

    coreCounter++;

    if (coreCounter == config.numCpu) {
        coreCounter = 0;
    }

    generateProcess(processId, std::move(processName), assignedCore, true);
}

void Scheduler::generateMultipleProcesses() {
    for (uint32_t i = 1; i <= 10 && running && generatorRunning; ++i) {
        std::ostringstream oss;
        oss << "process" << std::setw(2) << std::setfill('0') << i;

        std::lock_guard<std::mutex> lock(counter_mutex);

        uint32_t processId = processCounter++;
        uint8_t assignedCore = coreCounter;

        coreCounter++;
        if (coreCounter == config.numCpu) {
            coreCounter = 0;
        }

        generateProcess(processId, oss.str(), assignedCore, false);
    }
}

void Scheduler::stopGenerator() {
    generatorRunning = false;
}

void Scheduler::generateProcess(uint32_t processId, std::string processName, uint8_t coreNumber, bool registerScreen) { 
    std::shared_ptr<Process> process = std::make_shared<Process>(processId, processName, coreNumber);

    if (registerScreen) {
        if (!ConsoleManager::getInstance()->registerScreen(processName, true)) {
            return;
        }

        ConsoleManager::getInstance()->consoleTable[processName]->setProcess(process);
    }

    std::unique_lock lock(process_mutex);
    processList.push_back(process);
    if (!cpuReadyQueues.empty()) {
        cpuReadyQueues[coreNumber].push(process);
    }
}

void Scheduler::runScheduler(uint8_t coreNumber) {
    while(running) {
        if (!isReadyQueueEmpty(coreNumber)) {
            Process& process = getProcess(coreNumber);

            while (!process.isFinished() && running) {
                process.executeInstruction();

                if (config.delaysPerExec > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.delaysPerExec));
                }
            }

            dequeueProcess(coreNumber);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

bool Scheduler::isReadyQueueEmpty(uint8_t coreNumber) {
    std::shared_lock lock(process_mutex);
    return cpuReadyQueues[coreNumber].empty();
}

Process& Scheduler::getProcess(uint8_t coreNumber) {
    std::shared_lock lock(process_mutex);
    return *cpuReadyQueues[coreNumber].front();
}

void Scheduler::dequeueProcess(uint8_t coreNumber) {
    std::unique_lock lock(process_mutex);
    if (!cpuReadyQueues[coreNumber].empty()) {
        cpuReadyQueues[coreNumber].pop();
    }
}

void Scheduler::displayProcessList() {
    std::shared_lock lock(process_mutex);

    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Running processes:" << std::endl;

    for (const auto& process : processList) {
        if (!process->isFinished()) {
            std::cout << std::left << std::setw(16) << process->getProcessName()
                      << "(" << process->getCreationTime() << ")"
                      << "    Core: " << static_cast<int>(process->getCoreNumber())
                      << "    " << (process->getLineNumber() - 1)
                      << " / " << process->getInstructionSize()
                      << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "Finished processes:" << std::endl;

    for (const auto& process : processList) {
        if (process->isFinished()) {
            std::cout << std::left << std::setw(16) << process->getProcessName()
                      << "(" << process->getCreationTime() << ")"
                      << "    Finished"
                      << "    " << process->getInstructionSize()
                      << " / " << process->getInstructionSize()
                      << std::endl;
        }
    }

    std::cout << "----------------------------------------------" << std::endl;
}