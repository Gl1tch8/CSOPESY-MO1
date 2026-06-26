#pragma once
#include "../../src/interfaces/Command.hpp"
#include "../../include/services/ReportUtilService.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class ReportUtilCommand : public Command {
public:
    // Accept the ReportUtilService pointer from main.cpp
    ReportUtilCommand(ReportUtilService* service);

    void print(std::string log);

    void execute(std::string input);
};