#include <string>
#include <vector>
#include "SymbolTable.hpp"

class Helper {
public:
    std::string parse(std::string input);
    static std::string getFormattedTime(const std::string& format);
};

std::vector<Instruction> customInstructions(std::string processName, std::string filepath, uint32_t maxIns);