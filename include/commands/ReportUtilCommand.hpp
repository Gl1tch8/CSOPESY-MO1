#include "../../src/interfaces/Command.hpp"
#include "../../include/services/ScreenService.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class ReportUtilCommand : public Command {
public:
    // Accept the ReportUtilService pointer from main.cpp
    ReportUtilCommand(ScreenService* service);

    void print(std::string log);

    void execute(std::string input);
};