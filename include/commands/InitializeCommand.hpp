#pragma once
#include "../../src/interfaces/Command.hpp"
#include "../../include/services/InitializeService.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class InitializeCommand : public Command {
public:
    // Accept the InitializeService pointer from main.cpp
    InitializeCommand(InitializeService* service);

    void print(std::string log);

    void execute(std::string input);
};