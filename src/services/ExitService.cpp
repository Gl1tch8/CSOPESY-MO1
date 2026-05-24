#include "../include/serviceshpp/ExitService.hpp"

ExitService::ExitService() :Service() {}

std::string ExitService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}