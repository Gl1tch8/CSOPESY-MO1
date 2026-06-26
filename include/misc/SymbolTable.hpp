#pragma once

#include <unordered_map>
#include<vector>
#include <string>
#include <cstdint>
#include <atomic>

#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <sstream>

enum class OperationCode {
    PRINT,
    DECLARE,
    ADD,
    SUBTRACT,
    SLEEP,
    FOR
};


struct Instruction {
    OperationCode opCode;
    std::vector<std::string> operands;
    std::vector<Instruction> body; // FOR loops
    uint8_t repeats = 0; // for loops

};

class SymbolTable {
    public:
        SymbolTable();

        void setSymbol(std::string name, uint16_t value);
        uint16_t getSymbol(std::string name);

    private:
        std::unordered_map<std::string, uint16_t> variableTable;

};

class InstructionParser {
public:
    InstructionParser(SymbolTable& symbolTable, std::atomic<bool>& running, const std::string& processName);
    void executeBlock(const std::vector<Instruction>& instructions, uint64_t& tick);
    const std::vector<std::string>& getOutput() const;

    //parse the instruction string and return an Instruction struct
    Instruction parse(std::string line);

    //execute the instruction 
    void execute(Instruction instruction);

    //print opCode
    void print(std::string message);

    //declare a variable with name and value
    void declare(std::string varName = "var", std::string value = "0");

    //add two variables and return the result
    uint16_t add(uint16_t varOne = 0, uint16_t varTwo = 0);

    //subtract two variables and return the result
    uint16_t subtract(uint16_t varOne = 0, uint16_t varTwo = 0);

    //sleep for a certain duration in cpu ticks
    void sleep(uint8_t duration);

    //loop for a certain number of iterations and execute the body of the loop
    void forLoop(std::vector<Instruction> body, uint16_t iterations);

private:
    SymbolTable& symbolTable;
    std::atomic<bool>& running;
    std::string processName;
    std::vector<std::string> outputLog;
    uint16_t resolveValue(const std::string& operand);
};


