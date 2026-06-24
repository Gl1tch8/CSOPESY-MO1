#include "../../include/misc/Process.hpp"
#include "Core.hpp"
// Compatibility constructor
Process::Process(std::string name, int pid, std::string startTime, int coreId, int currentLine, int totalLines, bool isFinished)
    : startTimeStr(startTime) {
    info.name = name;
    info.pid = pid;
    info.coreId = coreId;
    info.currentLineIndex = currentLine;
    info.totalLines = totalLines;
    info.state = isFinished ? ProcessState::FINISHED : ProcessState::RUNNING;
    info.arrivalTime = 0;
    info.burstTime = 0;
    info.startTime = 0;
    info.endTime = 0;
}

int Process::getPid() const { return info.pid; }
void Process::setPid(int pid) { info.pid = pid; }

int Process::getCoreId() const { return info.coreId; }
void Process::setCoreId(int coreId) { info.coreId = coreId; }

std::string Process::getName() const { return info.name; }
void Process::setName(const std::string& name) { info.name = name; }

CPUTick Process::getArrivalTime() const { return info.arrivalTime; }
void Process::setArrivalTime(CPUTick arrivalTime) { info.arrivalTime = arrivalTime; }

CPUTick Process::getBurstTime() const { return info.burstTime; }
void Process::setBurstTime(CPUTick burstTime) { info.burstTime = burstTime; }

CPUTick Process::getStartTime() const { return info.startTime; }
void Process::setStartTime(CPUTick startTime) { info.startTime = startTime; }

CPUTick Process::getEndTime() const { return info.endTime; }
void Process::setEndTime(CPUTick endTime) { info.endTime = endTime; }

int Process::getCurrentLineIndex() const { return info.currentLineIndex; }
void Process::setCurrentLineIndex(int index) { info.currentLineIndex = index; }

int Process::getTotalLines() const { return info.totalLines; }
void Process::setTotalLines(int lines) { info.totalLines = lines; }

ProcessState Process::getState() const { return info.state; }
void Process::setState(ProcessState state) { info.state = state; }

const std::vector<Instruction>& Process::getInstructions() const { return info.instructions; }
void Process::setInstructions(const std::vector<Instruction>& instructions) { info.instructions = instructions; }

const SymbolTable& Process::getSymbolTable() const { return info.symbolTable; }
SymbolTable& Process::getSymbolTable() { return info.symbolTable; }
void Process::setSymbolTable(const SymbolTable& symbolTable) { info.symbolTable = symbolTable; }

// Compatibility methods
int Process::getCurrentLine() const { return info.currentLineIndex; }
void Process::setCurrentLine(int line) { info.currentLineIndex = line; }

bool Process::getIsFinished() const { return info.state == ProcessState::FINISHED; }
void Process::setIsFinished(bool finished) {
    if (finished) {
        info.state = ProcessState::FINISHED;
    } else if (info.state == ProcessState::FINISHED) {
        info.state = ProcessState::READY;
    }
}

std::string Process::getStartTimeStr() const {
    if (!startTimeStr.empty()) {
        return startTimeStr;
    }
    return std::to_string(info.startTime);
}
