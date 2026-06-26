#pragma once
#include "../../src/interfaces/Command.hpp"
#include "../../include/services/ExitService.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class ExitCommand : public Command {
public:
    // Accept the ExitService pointer from main.cpp
    ExitCommand(ExitService* service);

    void print(std::string log);

    void execute(std::string input);
};