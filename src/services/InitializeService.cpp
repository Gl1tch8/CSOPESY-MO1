#include "../include/serviceshpp/InitializeService.hpp"

InitializeService::InitializeService() :Service() {}

std::string InitializeService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}