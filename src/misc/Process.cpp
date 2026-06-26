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

    // datetime of process
    if (startTimeStr.empty()) {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        
        #ifdef _WIN32
            localtime_s(&tm, &t);
        #else
            localtime_r(&t, &tm);
        #endif

        char timeBuf[32];
        std::strftime(timeBuf, sizeof(timeBuf), "%m/%d/%Y %I:%M:%S%p", &tm);
        startTimeStr = std::string(timeBuf);
    }
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

void Process::appendOutput(const std::string& line) {
    // add Datetime to process logs
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "(%m/%d/%Y %I:%M:%S%p)", &tm);

    std::ostringstream entry;
    entry << timeBuf << " Core:" << info.coreId << " \"" << line << "\"";

    std::unique_lock lock(outputMutex);
    outputLog.push_back(entry.str());
}

std::string Process::getOutput() const {
    std::unique_lock lock(outputMutex);
    std::ostringstream ss;
    for (const auto& line : outputLog) {
        ss << line << "\n";
    }
    return ss.str();
}

static uint16_t clamp16(uint32_t val) {
    return static_cast<uint16_t>(std::min(val, static_cast<uint32_t>(UINT16_MAX)));
}

void Process::executeInstructions(uint64_t& tick, std::atomic<bool>& running) {
    SymbolTable& sym = info.symbolTable;
    executeBlock(info.instructions, sym, tick, running);
}

void Process::executeBlock(const std::vector<Instruction>& instructions, SymbolTable& sym, uint64_t& tick, std::atomic<bool>& running) {
    for (const auto& instr : instructions) {
        if (!running) return;

        switch (instr.opCode) {

        case OperationCode::DECLARE: {
            std::string var = instr.operands[0];
            uint16_t val = static_cast<uint16_t>(std::stoul(instr.operands[1]));
            sym.setSymbol(var, val);
            info.currentLineIndex++;
            tick++;
            break;
        }

        case OperationCode::PRINT: {
            std::string msg = instr.operands[0];
            // Replace variable reference if present
            // Format: "Hello world from <name>!" or "Value from: " + var
            if (!msg.empty() && msg[0] != '"') {
                // it's a variable
                uint16_t val = sym.getSymbol(msg);
                appendOutput(std::to_string(val));
            } else {
                // strip quotes
                std::string out = msg;
                if (out.size() >= 2 && out.front() == '"' && out.back() == '"') {
                    out = out.substr(1, out.size() - 2);
                }
                appendOutput(out);
            }
            info.currentLineIndex++;
            tick++;
            break;
        }

        case OperationCode::ADD: {
            std::string dest = instr.operands[0];
            uint16_t a = isdigit(instr.operands[1][0]) 
                ? static_cast<uint16_t>(std::stoul(instr.operands[1])) 
                : sym.getSymbol(instr.operands[1]);
            uint16_t b = isdigit(instr.operands[2][0]) 
                ? static_cast<uint16_t>(std::stoul(instr.operands[2])) 
                : sym.getSymbol(instr.operands[2]);
            sym.setSymbol(dest, clamp16(static_cast<uint32_t>(a) + static_cast<uint32_t>(b)));
            info.currentLineIndex++;
            tick++;
            break;
        }

        case OperationCode::SUBTRACT: {
            std::string dest = instr.operands[0];
            uint16_t a = isdigit(instr.operands[1][0]) 
                ? static_cast<uint16_t>(std::stoul(instr.operands[1])) 
                : sym.getSymbol(instr.operands[1]);
            uint16_t b = isdigit(instr.operands[2][0]) 
                ? static_cast<uint16_t>(std::stoul(instr.operands[2])) 
                : sym.getSymbol(instr.operands[2]);
            uint32_t result = static_cast<uint32_t>(a) >= static_cast<uint32_t>(b) 
                ? a - b : 0; // clamp at 0
            sym.setSymbol(dest, static_cast<uint16_t>(result));
            info.currentLineIndex++;
            tick++;
            break;
        }

        case OperationCode::SLEEP: {
            uint8_t sleepTicks = static_cast<uint8_t>(std::stoul(instr.operands[0]));
            for (uint8_t s = 0; s < sleepTicks && running; ++s) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                tick++;
            }
            info.currentLineIndex++;
            break;
        }

        case OperationCode::FOR: {
            uint8_t repeats = instr.repeats;
            for (uint8_t r = 0; r < repeats && running; ++r) {
                executeBlock(instr.body, sym, tick, running);
            }
            info.currentLineIndex++;
            break;
        }

        }
    }
}
