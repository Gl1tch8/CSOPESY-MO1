#include "../../include/services/SchedulerService.hpp"
#include "../../include/misc/SystemState.hpp"
#include "../../include/services/ConfigService.hpp"

#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>

SchedulerService::SchedulerService() : Service() {}

std::string SchedulerService::executeFlags(std::string input) {
    if (input.find("scheduler-start") != std::string::npos) {
        start();
        return "Scheduler started.";
    }

    if (input.find("scheduler-stop") != std::string::npos) {
        stop();
        return "Scheduler stopped.";
    }

    return "Error: unrecognized scheduler command.";
}

void SchedulerService::start() {
    if (running) {
        return;
    }

    ConfigService configService;
    configService.parseConfigFile();
    
    config = configService.getConfig();

    SystemState::getInstance().initializeCores(config.cpuCount);
    cpuReadyQueues.resize(config.cpuCount);
    
    running = true;
    generating = true;

    for (int i = 0; i < static_cast<int>(config.cpuCount); ++i) {
        cpuThreads.emplace_back(&SchedulerService::runCpuCore, this, i);
    }

    generatorThread = std::thread(&SchedulerService::generateProcessor, this);
}

void SchedulerService::stop() {
    generating = false;

    if (generatorThread.joinable()) {
        generatorThread.join();
    }

    // for (auto& t : cpuThreads) {
    //     if (t.joinable()) {
    //         t.join();
    //     }
    // }

    // cpuThreads.clear();
}

void SchedulerService::run() {}

void SchedulerService::generateProcessor() {

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> insDist(config.minIns, config.maxIns);
    std::uniform_int_distribution<int> opDist(0, 4); // DECLARE, PRINT, ADD, SUBTRACT, SLEEP
    std::uniform_int_distribution<int> valDist(0, 100);
    std::uniform_int_distribution<uint8_t> sleepDist(1, 10);
    std::uniform_int_distribution<uint8_t> repeatDist(2, 5);
    std::uniform_int_distribution<int> forChance(0, 4); // 1 in 5 chance of FOR

    std::function<std::vector<Instruction>(const std::string&, int, int)> makeBlock =
        [&](const std::string& name, int count, int depth) -> std::vector<Instruction> {
        std::vector<Instruction> block;
        for (int k = 0; k < count; ++k) {
            if (depth < 3 && forChance(rng) == 0) {
                Instruction forInstr;
                forInstr.opCode = OperationCode::FOR;
                forInstr.repeats = repeatDist(rng);
                forInstr.body = makeBlock(name, std::uniform_int_distribution<int>(2, 4)(rng), depth + 1);
                block.push_back(forInstr);
            } else {
                int op = opDist(rng);
                Instruction instr;
                if (op == 0) {
                    instr.opCode = OperationCode::DECLARE;
                    instr.operands = {"x", std::to_string(valDist(rng))};
                } else if (op == 1) {
                    instr.opCode = OperationCode::PRINT;
                    instr.operands = {"\"Hello world from " + name + "!\""};
                } else if (op == 2) {
                    instr.opCode = OperationCode::ADD;
                    instr.operands = {"x", "x", std::to_string(valDist(rng))};
                } else if (op == 3) {
                    instr.opCode = OperationCode::SUBTRACT;
                    instr.operands = {"x", "x", std::to_string(valDist(rng))};
                } else {
                    instr.opCode = OperationCode::SLEEP;
                    instr.operands = {std::to_string(sleepDist(rng))};
                }
                block.push_back(instr);
            }
        }
        return block;
    };


    while (generating) {
        uint64_t currentTick = SystemState::getInstance().getSystemTime();
        if (currentTick % config.batchProcessFreq == 0) {
            int pid = static_cast<int>(processCounter++);

            std::ostringstream nameStream;
            nameStream << "p" << std::setfill('0') << std::setw(2) << pid;
            std::string name = nameStream.str();

            ProcessInfo info;
            info.pid = pid;
            info.name = name;
            info.arrivalTime = currentTick;
            info.burstTime = 0;
            info.startTime = 0;
            info.endTime = 0;
            info.currentLineIndex = 0;
            info.state = ProcessState::NEW;

            int numInstructions = insDist(rng);
            info.instructions = makeBlock(name, numInstructions, 0);
            info.totalLines = static_cast<int>(info.instructions.size());

            info.coreId = 0;

            {
                std::unique_lock lock(queueMutex);
                
                if (config.schedulingAlgo == "rr") {
                    // distribute evenly across cores
                    info.coreId = nextCoreAssignment;
                    nextCoreAssignment = (nextCoreAssignment + 1) % static_cast<int>(config.cpuCount);
                }
                else if (config.schedulingAlgo == "fcfs") {
                    int shortestCore = 0;
                    size_t minSize = std::numeric_limits<size_t>::max();
                    
                    // load balancer
                    for (int i = 0; i < static_cast<int>(cpuReadyQueues.size()); ++i) {
                        if (cpuReadyQueues[i].size() < minSize) {
                            minSize = cpuReadyQueues[i].size();
                            shortestCore = i;
                        }
                    }
                    info.coreId = shortestCore;
                }
                info.arrivalTime = currentTick;

                // push to chosen q
                if (info.coreId >= 0 && info.coreId < static_cast<int>(cpuReadyQueues.size())) {
                    Process process(info);
                    cpuReadyQueues[info.coreId].push(process);
                    SystemState::getInstance().addProcess(process);
                }
            }

        }

        SystemState::getInstance().incrementSystemTime();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


void SchedulerService::runCpuCore(int coreId) {
    while (running) {
        int processPid = -1;
        bool hasProcess = false;

        {
            std::unique_lock lock(queueMutex);
            if (!cpuReadyQueues[coreId].empty()) {
                Process& queuedProcess = cpuReadyQueues[coreId].front();
                processPid = queuedProcess.getPid();
                cpuReadyQueues[coreId].pop();
                hasProcess = true;
            }
        }

        if (hasProcess && processPid >= 0) {
            std::shared_ptr<Process> process = SystemState::getInstance().getProcessByPid(processPid);
            if (process) {
                // core init
                SystemState::getInstance().setCoreActive(coreId, true);
                SystemState::getInstance().setCoreProcess(coreId, process.get());

                process->setState(ProcessState::READY);
                process->setState(ProcessState::RUNNING);

                InstructionParser parser(process->getSymbolTable(), running, process->getName());
                const auto& instructions = process->getInstructions();

                // time quantum for rr
                int quantum = (config.schedulingAlgo == "rr") ? config.quantumCycles : static_cast<int>(instructions.size());
                int linesExecuted = 0;
                int startIndex = process->getCurrentLineIndex();
                
                const auto& output = parser.getOutput(); // get output log immediately
                for (int i = startIndex; i < static_cast<int>(instructions.size()) && running; ++i) {
                    uint64_t currentTick = SystemState::getInstance().getSystemTime();
                    parser.executeBlock({instructions[i]}, currentTick);
                    process->setCurrentLineIndex(i + 1);
                    linesExecuted++;

                    if (config.delayPerSec > 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(config.delayPerSec));
                    }

                    if (linesExecuted >= quantum) {
                        break; // time-quantum limit
                    }
                }

                for (const auto& line : parser.getOutput()) {
                    process->appendOutput(line);
                }

                if (process->getCurrentLineIndex() < static_cast<int>(instructions.size())) { // is process done?
                    // put back in queue
                    process->setState(ProcessState::READY);
                    std::unique_lock lock(queueMutex);
                    cpuReadyQueues[coreId].push(*process); 
                } else {
                    process->setState(ProcessState::FINISHED);
                    process->setEndTime(SystemState::getInstance().getSystemTime());
                }

                SystemState::getInstance().setCoreActive(coreId, false);
                SystemState::getInstance().setCoreProcess(coreId, nullptr);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}