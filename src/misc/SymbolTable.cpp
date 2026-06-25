#include "SymbolTable.hpp";

SymbolTable::SymbolTable() {
}

uint16_t SymbolTable::getSymbol(std::string name) {
    return this->variableTable[name];
}

void SymbolTable::setSymbol(std::string name, uint16_t value) {
    this->variableTable.emplace(name,value);
}


InstructionParser::InstructionParser(SymbolTable& symbolTable, std::atomic<bool>& running, const std::string& processName)
    : symbolTable(symbolTable), running(running), processName(processName) {}

const std::vector<std::string>& InstructionParser::getOutput() const {
    return outputLog;
}

// Resolves an operand: if it looks like a number, parse it; otherwise treat as variable name
uint16_t InstructionParser::resolveValue(const std::string& operand) {
    if (operand.empty()) return 0;
    if (std::isdigit(static_cast<unsigned char>(operand[0]))) {
        return static_cast<uint16_t>(std::stoul(operand));
    }
    return symbolTable.getSymbol(operand);
}

void InstructionParser::executeBlock(const std::vector<Instruction>& instructions, uint64_t& tick) {
    for (const auto& instr : instructions) {
        if (!running) return;

        switch (instr.opCode) {

        case OperationCode::DECLARE:
            declare(instr.operands[0], instr.operands[1]);
            tick++;
            break;

        case OperationCode::PRINT:
            print(instr.operands[0]);
            tick++;
            break;

        case OperationCode::ADD: {
            uint16_t result = add(resolveValue(instr.operands[1]), resolveValue(instr.operands[2]));
            symbolTable.setSymbol(instr.operands[0], result);
            tick++;
            break;
        }

        case OperationCode::SUBTRACT: {
            uint16_t result = subtract(resolveValue(instr.operands[1]), resolveValue(instr.operands[2]));
            symbolTable.setSymbol(instr.operands[0], result);
            tick++;
            break;
        }

        case OperationCode::SLEEP:
            sleep(static_cast<uint8_t>(std::stoul(instr.operands[0])));
            tick += static_cast<uint8_t>(std::stoul(instr.operands[0]));
            break;

        case OperationCode::FOR:
            forLoop(instr.body, instr.repeats);
            tick++;
            break;
        }
    }
}


void InstructionParser::print(std::string message) {
    std::string out = message;

    // Strip surrounding quotes if present
    if (out.size() >= 2 && out.front() == '"' && out.back() == '"') {
        out = out.substr(1, out.size() - 2);
    }

    // Handle "string" +var concatenation (e.g. "Value from: " +x)
    auto plusPos = out.find("\" +");
    if (plusPos == std::string::npos) plusPos = out.find("\"+");
    if (plusPos != std::string::npos) {
        std::string strPart = message.substr(1, plusPos - 1); // strip leading quote
        std::string varPart = message.substr(message.find('+') + 1);
        varPart.erase(0, varPart.find_first_not_of(" \t\""));
        varPart.erase(varPart.find_last_not_of(" \t\")") + 1);
        uint16_t val = symbolTable.getSymbol(varPart);
        outputLog.push_back(strPart + std::to_string(val));
        return;
    }

    outputLog.push_back(out);
}

void InstructionParser::declare(std::string varName, std::string value) {
    uint16_t val = static_cast<uint16_t>(std::stoul(value));
    symbolTable.setSymbol(varName, val);
}

uint16_t InstructionParser::add(uint16_t varOne, uint16_t varTwo) {
    uint32_t result = static_cast<uint32_t>(varOne) + static_cast<uint32_t>(varTwo);
    return static_cast<uint16_t>(std::min(result, static_cast<uint32_t>(UINT16_MAX)));
}

uint16_t InstructionParser::subtract(uint16_t varOne, uint16_t varTwo) {
    return varOne >= varTwo ? varOne - varTwo : 0; // clamp at 0
}

void InstructionParser::sleep(uint8_t duration) {
    for (uint8_t i = 0; i < duration && running; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void InstructionParser::forLoop(std::vector<Instruction> body, uint16_t iterations) {
    uint64_t dummyTick = 0;
    for (uint16_t i = 0; i < iterations && running; ++i) {
        executeBlock(body, dummyTick);
    }
}

// parse and execute are stubs — instructions are generated programmatically, not parsed from text
Instruction InstructionParser::parse(std::string line) {
    Instruction instr;
    instr.opCode = OperationCode::PRINT;
    instr.operands.push_back(line);
    return instr;
}

void InstructionParser::execute(Instruction instruction) {
    uint64_t dummyTick = 0;
    executeBlock({instruction}, dummyTick);
}







