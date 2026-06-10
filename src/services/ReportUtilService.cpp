#include "../include/services/ReportUtilService.hpp"

ReportUtilService::ReportUtilService() :Service() {}

std::string ReportUtilService::executeFlags(std::string input) {
    return input + " command recognized. Doing something.";
}