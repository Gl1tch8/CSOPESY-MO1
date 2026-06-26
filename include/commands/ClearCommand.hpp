#pragma once
#include "../../src/interfaces/Command.hpp"
#include "../../include/services/ClearService.hpp"
#include "../../include/tui/MainMenu.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class ClearCommand : public Command {
public:
    // Accept the ClearService pointer from main.cpp
    ClearCommand(ClearService* service);

    void print(std::string log);

    void execute(std::string input);
};