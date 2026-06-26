#include "../../include/misc/Helper.hpp"
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

std::string Helper::getFormattedTime(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    
    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    char timeBuf[64];
    std::strftime(timeBuf, sizeof(timeBuf), format.c_str(), &tm);
    return std::string(timeBuf);
}


std::string Helper::parse(std::string input) {
    // parse first word only and return that
    std::string command = input.substr(0, input.find(" "));
    return command;
}

namespace {
    // Helper to split a string by commas, ignoring commas inside parentheses, brackets, and quotes
    std::vector<std::string> splitInstructions(const std::string& str) {
        std::vector<std::string> result;
        std::string current;
        int parenDepth = 0;
        bool inQuotes = false;
        for (size_t i = 0; i < str.length(); ++i) {
            char c = str[i];
            if (c == '"' && (i == 0 || str[i - 1] != '\\')) {
                inQuotes = !inQuotes;
                current += c;
            } else if (!inQuotes && (c == '(' || c == '[')) {
                parenDepth++;
                current += c;
            } else if (!inQuotes && (c == ')' || c == ']')) {
                parenDepth--;
                current += c;
            } else if (!inQuotes && parenDepth == 0 && c == ',') {
                size_t start = current.find_first_not_of(" \t");
                size_t end = current.find_last_not_of(" \t");
                if (start != std::string::npos && end != std::string::npos) {
                    result.push_back(current.substr(start, end - start + 1));
                }
                current.clear();
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            size_t start = current.find_first_not_of(" \t");
            size_t end = current.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                result.push_back(current.substr(start, end - start + 1));
            }
        }
        return result;
    }

    // Forward declaration to allow mutual recursion
    Instruction parseSingleInstruction(const std::string& str, uint32_t maxIns);

    Instruction parseForInstruction(const std::string& str, uint32_t maxIns) {
        Instruction instr;
        instr.opCode = OperationCode::FOR;

        size_t firstBracket = str.find('[');
        size_t lastBracket = str.rfind(']');

        if (firstBracket != std::string::npos && lastBracket != std::string::npos && lastBracket > firstBracket) {
            std::string bodyString = str.substr(firstBracket + 1, lastBracket - firstBracket - 1);
            std::vector<std::string> innerStrList = splitInstructions(bodyString);
            for (const auto& innerStr : innerStrList) {
                instr.body.push_back(parseSingleInstruction(innerStr, maxIns));
            }

            instr.repeats = static_cast<uint8_t>(std::min(maxIns, static_cast<uint32_t>(255)));
        }
        return instr;
    }

    Instruction parseSingleInstruction(const std::string& str, uint32_t maxIns) {
        Instruction instr;
        size_t openParen = str.find('(');
        if (openParen == std::string::npos) {
            instr.opCode = OperationCode::PRINT;
            instr.operands.push_back(str);
            return instr;
        }

        std::string opName = str.substr(0, openParen);
        opName.erase(0, opName.find_first_not_of(" \t\r\n"));
        opName.erase(opName.find_last_not_of(" \t\r\n") + 1);

        size_t closeParen = str.rfind(')');
        std::string inner;
        if (closeParen != std::string::npos && closeParen > openParen) {
            inner = str.substr(openParen + 1, closeParen - openParen - 1);
        } else {
            inner = str.substr(openParen + 1);
        }

        if (opName == "PRINT") {
            instr.opCode = OperationCode::PRINT;
            instr.operands.push_back(inner);
        } else if (opName == "DECLARE") {
            instr.opCode = OperationCode::DECLARE;
            std::vector<std::string> parts = splitInstructions(inner);
            for (auto& p : parts) {
                instr.operands.push_back(p);
            }
        } else if (opName == "ADD") {
            instr.opCode = OperationCode::ADD;
            std::vector<std::string> parts = splitInstructions(inner);
            for (auto& p : parts) {
                instr.operands.push_back(p);
            }
        } else if (opName == "SUBTRACT") {
            instr.opCode = OperationCode::SUBTRACT;
            std::vector<std::string> parts = splitInstructions(inner);
            for (auto& p : parts) {
                instr.operands.push_back(p);
            }
        } else if (opName == "SLEEP") {
            instr.opCode = OperationCode::SLEEP;
            instr.operands.push_back(inner);
        } else if (opName == "FOR") {
            instr = parseForInstruction(str, maxIns);
        } else {
            instr.opCode = OperationCode::PRINT;
            instr.operands.push_back(str);
        }

        return instr;
    }
}

std::vector<Instruction> customInstructions(std::string processName, std::string filepath, uint32_t maxIns) {
    std::vector<Instruction> instructions;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return instructions;
    }
    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty()) continue;

        if (line.rfind("FOR", 0) == 0) {
            instructions.push_back(parseForInstruction(line, maxIns));
        } else {
            instructions.push_back(parseSingleInstruction(line, maxIns));
        }
    }
    return instructions;
}

