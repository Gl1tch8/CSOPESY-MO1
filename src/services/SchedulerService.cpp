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
    const Config& config = configService.getConfig();

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
    ConfigService configService;
    configService.parseConfigFile();
    const Config& config = configService.getConfig();

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

    uint64_t tick = 0;

    while (generating) {
        if (tick % config.batchProcessFreq == 0) {
            int pid = static_cast<int>(processCounter++);

            std::ostringstream nameStream;
            nameStream << "p" << std::setfill('0') << std::setw(2) << pid;
            std::string name = nameStream.str();

            ProcessInfo info;
            info.pid = pid;
            info.name = name;
            info.arrivalTime = tick;
            info.burstTime = 0;
            info.startTime = 0;
            info.endTime = 0;
            info.currentLineIndex = 0;
            info.state = ProcessState::NEW;

            int numInstructions = insDist(rng);
            info.instructions = makeBlock(name, numInstructions, 0);
            info.totalLines = static_cast<int>(info.instructions.size());

            info.coreId = nextCoreAssignment;
            nextCoreAssignment = (nextCoreAssignment + 1) % static_cast<int>(config.cpuCount);

            Process process(info);

            {
                std::unique_lock lock(queueMutex);
                if (info.coreId >= 0 && info.coreId < static_cast<int>(cpuReadyQueues.size())) {
                    cpuReadyQueues[info.coreId].push(process);
                }
            }

            SystemState::getInstance().addProcess(process);
        }

        ++tick;
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
            Process* process = SystemState::getInstance().getProcessByPid(processPid);
           if (process) {
                process->setState(ProcessState::READY);
                process->setState(ProcessState::RUNNING);

                uint64_t tick = 0;
                InstructionParser parser(process->getSymbolTable(), running, process->getName());
                parser.executeBlock(process->getInstructions(), tick);

                // Store output back into process
                for (const auto& line : parser.getOutput()) {
                    process->appendOutput(line);
                }

                process->setState(ProcessState::FINISHED);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}