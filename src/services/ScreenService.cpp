#include "../include/serviceshpp/ScreenService.hpp"

ScreenService::ScreenService() :Service() {}

std::string ScreenService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}