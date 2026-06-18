#include "Process.h"

Process::Process(uint32_t pId, std::string name, uint8_t coreNumber) {
    this->pId = pId;
    this->name = name;
    this->coreNumber = coreNumber;
    this->logFilePath = name + ".txt";

    // ADD THIS BLOCK
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y %I:%M:%S%p");
    this->creationTime = oss.str();

    instructions = std::make_unique<InstructionQueue>();
    symbolTable = std::make_unique<SymbolTable>();
    logger = std::make_unique<Logger>();

    generateInstructions();
}

void Process::executeInstruction() {
    std::unique_lock lock(mtx);
    if (instructions->empty()) {
        return;
    }

    instructions->front()->execute();

    // checks whether instruction is FOR or SLEEP and is still running before removing the instruction
    if (!instructions->front()->isLooping()) {
        instructions->pop();
        lineNumber++;
    }

    if (instructions->empty() && !logFileWritten) {
        writeLogFile();
    }
}

void Process::displayLog() {
    std::shared_lock lock(mtx);
    std::cout << "Logs:" << std::endl;
    for (const auto& log : *logger) {
        std::cout << "(" << std::get<0>(log) << ") Core:" << static_cast<int>(std::get<1>(log)) << " \"" << std::get<2>(log) << "\"" << std::endl;
    }
}

bool Process::isFinished() {
    std::shared_lock lock(mtx);
    return instructions->empty();
}

uint32_t Process::getpId() {
    return pId;
}

std::string Process::getProcessName() {
    return name;
}

uint32_t Process::getInstructionSize() {
    return instructionCount;
}

uint32_t Process::getLineNumber() {
    return lineNumber;
}

uint8_t Process::getCoreNumber() {
    return coreNumber;
}

void Process::writeLogFile() {
    std::ofstream file(logFilePath, std::ios::trunc);

    if (!file.is_open()) {
        return;
    }

    file << "Process name: " << name << std::endl;
    file << std::endl;
    file << "Logs:" << std::endl;
    file << std::endl;

    for (const auto& log : *logger) {
        file << "(" << std::get<0>(log) << ") Core:" << static_cast<int>(std::get<1>(log)) << " \"" << std::get<2>(log) << "\"" << std::endl;
    }

    logFileWritten = true;
}

std::string Process::getCreationTime() {
    return creationTime;
}

void Process::generateInstructions() {
    symbolTable->emplace("var1", 0);
    symbolTable->emplace("var2", 0);
    symbolTable->emplace("var3", 0);
    symbolTable->emplace("var4", 0);
    symbolTable->emplace("var5", 0);
    symbolTable->emplace("var6", 0);
    
    for (uint32_t i = 0; i < 100; ++i) {
        instructions->push(std::make_unique<IPrint>("Hello world from " + name + "!", *symbolTable, *logger, coreNumber));
        instructionCount++;
    }
}

/*
    PRINT(msg)
    DECLARE(var1, 5)
    DECLARE(var2, 10)
    DECLARE(var3, 2)
    ADD(var4, var1, var2)
    SUBTRACT(var5, var2, var3)
    PRINT(var4)
    PRINT(var5)
    SLEEP(10)
    FOR([FOR([ADD(var6, var6, var1)], 2), PRINT(var6)]3)
*/