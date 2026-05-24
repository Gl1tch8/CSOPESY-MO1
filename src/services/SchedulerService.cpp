#include "../include/serviceshpp/SchedulerService.hpp"

SchedulerService::SchedulerService() :Service() {}

std::string SchedulerService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}