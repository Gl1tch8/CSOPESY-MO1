#include "../../include/services/SchedulerService.hpp"
#include "../../include/misc/SystemState.hpp"
#include "../../include/services/ConfigService.hpp"

#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <algorithm>

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

void SchedulerService::initScheduler() {
    if (running) {
        return; // cores already online
    }

    ConfigService configService;
    configService.parseConfigFile();

    config = configService.getConfig();

    SystemState::getInstance().initializeCores(config.cpuCount);
    cpuReadyQueues.resize(config.cpuCount);

    running = true;

    // bring the CPU cores online so processes can execute immediately,
    // even before "scheduler-start" begins generating dummy processes.
    for (int i = 0; i < static_cast<int>(config.cpuCount); ++i) {
        cpuThreads.emplace_back(&SchedulerService::runCpuCore, this, i);
    }
}

void SchedulerService::start() {
    if (!running) {
        initScheduler(); // safety: ensure cores are up
    }

    if (generating) {
        return; // generation already running
    }

    generating = true;
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

std::vector<Instruction> SchedulerService::generateInstructions(const std::string& name) {
    std::lock_guard<std::mutex> guard(genMutex); // rng shared with generator thread

    /*
    std::uniform_int_distribution<int> insDist(config.minIns, config.maxIns);
    std::uniform_int_distribution<int> opDist(0, 4); // DECLARE, PRINT, ADD, SUBTRACT, SLEEP
    std::uniform_int_distribution<int> valDist(0, 100);
    std::uniform_int_distribution<uint8_t> sleepDist(1, 10);
    std::uniform_int_distribution<uint8_t> repeatDist(2, 5);
    std::uniform_int_distribution<int> forChance(0, 4); // 1 in 5 chance of FOR

    const std::vector<std::string> varPool = {"x", "y", "z"};
    std::uniform_int_distribution<size_t> varDist(0, varPool.size() - 1);
    std::vector<std::string> declared;            // vars introduced so far (one symbol table per process)
    std::uniform_int_distribution<int> printChoice(0, 1);
    auto recordVar = [&](const std::string& v) {
        if (std::find(declared.begin(), declared.end(), v) == declared.end()) declared.push_back(v);
    };
    // ADD/SUBTRACT operands 2 and 3 may each be a variable or a literal value
    auto randOperand = [&]() -> std::string {
        if (printChoice(rng) == 0) {
            std::string var = varPool[varDist(rng)];
            recordVar(var); // variables auto-declare to 0 if not yet set
            return var;
        }
        return std::to_string(valDist(rng));
    };

    std::function<std::vector<Instruction>(const std::string&, int, int)> makeBlock =
        [&](const std::string& n, int count, int depth) -> std::vector<Instruction> {
        std::vector<Instruction> block;
        for (int k = 0; k < count; ++k) {
            if (depth < 3 && forChance(rng) == 0) {
                Instruction forInstr;
                forInstr.opCode = OperationCode::FOR;
                forInstr.repeats = repeatDist(rng);
                forInstr.body = makeBlock(n, std::uniform_int_distribution<int>(2, 4)(rng), depth + 1);
                block.push_back(forInstr);
            } else {
                int op = opDist(rng);
                Instruction instr;
                if (op == 0) {
                    std::string var = varPool[varDist(rng)];
                    instr.opCode = OperationCode::DECLARE;
                    instr.operands = {var, std::to_string(valDist(rng))};
                    recordVar(var);
                } else if (op == 1) {
                    instr.opCode = OperationCode::PRINT;
                    if (!declared.empty() && printChoice(rng) == 0) {
                        std::uniform_int_distribution<size_t> pick(0, declared.size() - 1);
                        instr.operands = {"\"Value from: \" +" + declared[pick(rng)]};
                    } else {
                        instr.operands = {"\"Hello world from " + n + "!\""};
                    }
                } else if (op == 2) {
                    std::string dest = varPool[varDist(rng)];
                    instr.opCode = OperationCode::ADD;
                    instr.operands = {dest, randOperand(), randOperand()};
                    recordVar(dest);
                } else if (op == 3) {
                    std::string dest = varPool[varDist(rng)];
                    instr.opCode = OperationCode::SUBTRACT;
                    instr.operands = {dest, randOperand(), randOperand()};
                    recordVar(dest);
                } else {
                    instr.opCode = OperationCode::SLEEP;
                    instr.operands = {std::to_string(sleepDist(rng))};
                }
                block.push_back(instr);
            }
        }
        return block;
    };

    int numInstructions = insDist(rng);
    return makeBlock(name, numInstructions, 0);
    */

    // CUSTOM INSTR GENERATION
    std::uniform_int_distribution<int> insDist(config.minIns, config.maxIns);
    std::uniform_int_distribution<int> addValDist(1, 10);

    int numInstructions = insDist(rng);
    std::vector<Instruction> instructions;
    for (int k = 0; k < numInstructions; ++k) {
        Instruction instr;
        if (k % 2 == 0) {
            instr.opCode = OperationCode::PRINT;
            instr.operands = {"\"Value from: \" +x"};
        } else {
            instr.opCode = OperationCode::ADD;
            instr.operands = {"x", "x", std::to_string(addValDist(rng))};
        }
        instructions.push_back(instr);
    }
    return instructions;
}

void SchedulerService::enqueueProcess(ProcessInfo info) {
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

    // push to chosen q
    if (info.coreId >= 0 && info.coreId < static_cast<int>(cpuReadyQueues.size())) {
        Process process(info);
        cpuReadyQueues[info.coreId].push(process);
        SystemState::getInstance().addProcess(process);
    }
}

std::string SchedulerService::createProcess(const std::string& name) {
    if (!running) {
        return "Error: system not initialized.";
    }
    if (SystemState::getInstance().getProcessByName(name) != nullptr) {
        return "Error: process " + name + " already exists.";
    }

    ProcessInfo info;
    info.pid = static_cast<int>(processCounter++);
    info.name = name;
    info.arrivalTime = SystemState::getInstance().getSystemTime();
    info.burstTime = 0;
    info.startTime = 0;
    info.endTime = 0;
    info.currentLineIndex = 0;
    info.state = ProcessState::NEW;
    info.coreId = 0;

    info.instructions = generateInstructions(name);
    info.totalLines = static_cast<int>(info.instructions.size());

    enqueueProcess(info);
    return "";
}

void SchedulerService::generateProcessor() {
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

            info.instructions = generateInstructions(name);
            info.totalLines = static_cast<int>(info.instructions.size());

            info.coreId = 0;
            enqueueProcess(info);
        }

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

                if (startIndex == 0) { // first execution: record start tick once
                    process->setStartTime(SystemState::getInstance().getSystemTime());
                }

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