#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <chrono> 
#include <ctime>

#include "Core.hpp"
#include "SymbolTable.hpp"
enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    FINISHED
};

struct ProcessInfo {
    int pid;
    int coreId;

    // name of process
    std::string name;

    // time when process arrived
    CPUTick arrivalTime;

    // time required for process to complete execution on CPU
    CPUTick burstTime;

    // time when process started execution on CPU
    CPUTick startTime;

    // time when process finished
    CPUTick endTime;

    // index of current instruction being executed
    int currentLineIndex;

    // total number of instructions in the program
    int totalLines;

    // process state (NEW, READY, RUNNING, WAITING, FINISHED)
    ProcessState state;

    // program instructions
    std::vector<Instruction> instructions;

    // symbols (variables)
    SymbolTable symbolTable;

    // logs process output
    std::vector<std::string> logs;
};

class Process {
public:
    Process(ProcessInfo info);

    // copy constructor
    Process(const Process& other)
    : info(other.info), startTimeStr(other.startTimeStr), outputLog(other.outputLog) {}

    // Compatibility constructor
    Process(std::string name, int pid, std::string startTime, int coreId, int currentLine, int totalLines, bool isFinished);

    // Getters and Setters
    int getPid() const;
    void setPid(int pid);

    int getCoreId() const;
    void setCoreId(int coreId);

    std::string getName() const;
    void setName(const std::string& name);

    CPUTick getArrivalTime() const;
    void setArrivalTime(CPUTick arrivalTime);

    CPUTick getBurstTime() const;
    void setBurstTime(CPUTick burstTime);

    CPUTick getStartTime() const;
    void setStartTime(CPUTick startTime);

    CPUTick getEndTime() const;
    void setEndTime(CPUTick endTime);

    int getCurrentLineIndex() const;
    void setCurrentLineIndex(int index);

    int getTotalLines() const;
    void setTotalLines(int lines);

    ProcessState getState() const;
    void setState(ProcessState state);

    const std::vector<Instruction>& getInstructions() const;
    void setInstructions(const std::vector<Instruction>& instructions);

    const SymbolTable& getSymbolTable() const;
    SymbolTable& getSymbolTable();
    void setSymbolTable(const SymbolTable& symbolTable);

    // Compatibility methods for old interface
    int getCurrentLine() const;
    void setCurrentLine(int line);
    bool getIsFinished() const;
    void setIsFinished(bool finished);
    std::string getStartTimeStr() const;

    void executeInstructions(uint64_t& tick, std::atomic<bool>& running);
    std::string getOutput() const;
    void appendOutput(const std::string& line);

    
private:
    ProcessInfo info;
    std::string startTimeStr;
    void executeBlock(const std::vector<Instruction>& instructions, SymbolTable& sym, uint64_t& tick, std::atomic<bool>& running);
    
    std::vector<std::string> outputLog;

    // mutable bc its used on getOutput, a const
    mutable std::mutex outputMutex;
};


// todo
class ProcessScheduler {
public:
    void schedule();

    void sort(std::vector<Process> *pList);
};
