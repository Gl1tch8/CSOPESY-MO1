#pragma once
#include "../../src/interfaces/Service.hpp"
#include <string>

class ReportUtilService : public Service {
public:
    ReportUtilService();

    // Reads the flags of the command and executes
    // the needed logic based on it
    // returns output log
    std::string executeFlags(std::string input);
};
