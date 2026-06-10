#include "../../src/interfaces/Command.hpp"
#include "../../include/services/SchedulerService.hpp"

// libraries for sleep
#include <chrono>
#include <thread>

class SchedulerCommand : public Command {
public:
    // Accept the SchedulerService pointer from main.cpp
    SchedulerCommand(SchedulerService* service);

    void print(std::string log);

    void execute(std::string input);

	void executeStart(std::string input);

	void executeStop(std::string input);
};