#include "../include/services/ExitService.hpp"

ExitService::ExitService() :Service() {}

std::string ExitService::executeFlags(std::string input) {
    return "Exiting...";
}