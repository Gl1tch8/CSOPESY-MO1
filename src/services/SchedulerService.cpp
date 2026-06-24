#include "../include/services/SchedulerService.hpp"

SchedulerService::SchedulerService() :Service() {}

std::string SchedulerService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}

// todo
void SchedulerService::start() {}

// todo
void SchedulerService::stop() {}

// todo
void SchedulerService::run() {}