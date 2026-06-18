#pragma once

#include "globals.h"
#include "ProcessInstructions.h"

using InstructionQueue = std::queue<std::unique_ptr<Instruction>>;

class Process {
    public:
        Process(uint32_t pId, std::string name, uint8_t coreNumber);
        ~Process() = default;

        void executeInstruction();
        void displayLog();
        bool isFinished();

        uint32_t getpId();
        std::string getProcessName();
        uint32_t getInstructionSize();
        uint32_t getLineNumber();
        uint8_t getCoreNumber();
        std::string getCreationTime();

    private:
        void generateInstructions();
        void writeLogFile();

        uint32_t pId;
        std::string name;
        uint8_t coreNumber;
        std::string logFilePath;
        std::string creationTime;

        std::atomic<uint32_t> lineNumber = 1;
        uint32_t instructionCount = 0;
        uint8_t sleepCounter;
        bool isLooping = 0;
        bool logFileWritten = false;

        std::unique_ptr<InstructionQueue> instructions;
        std::unique_ptr<SymbolTable> symbolTable;
        std::unique_ptr<Logger> logger;

        std::shared_mutex mtx;
};